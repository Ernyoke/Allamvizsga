#include "manageclients.h"
#include "ui_manageclients.h"

const int PORT = 10000;
const int SERVER_PORT = 40000;
const int MAXCLIENTS = 1024;

const int MIN_PORT = 10000;
const int MAX_PORT = 65000;

ManageClients::ManageClients(ClientModel *tableModel, ChannelModel *channelModel, QMutex *mutex) :
    PacketLogger(mutex)
{
    this->socket = new QUdpSocket(this);
    socket->bind(PORT);

    this->clientModel = tableModel;
    this->channelModel = channelModel;

    clientID = 0;

    connect(socket, SIGNAL(readyRead()), this, SLOT(readPendingDatagrams()));

    this->outPort = 10000;

    synchTimer = new QTimer(this);
    synchTimer->setInterval(10000);
    connect(synchTimer, SIGNAL(timeout()), this, SLOT(synchronizeClients()));
    synchTimer->start();
}

ManageClients::~ManageClients()
{
    qDebug() << "delete manageClients";
    synchTimer->stop();
    delete socket;
}

void ManageClients::readPendingDatagrams() {
    while(socket->hasPendingDatagrams()) {
        QByteArray dataReceived;
        dataReceived.resize(socket->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;
        socket->readDatagram(dataReceived.data(), dataReceived.size(), &sender, &senderPort);
        Datagram dgram(&dataReceived);
        this->processDatagram(dgram, sender, senderPort);
    }
}

void ManageClients::processDatagram(Datagram dgram, QHostAddress address, quint16 port) {
    createLogEntry(dgram);
    switch(dgram.getId()) {
    case Datagram::LOGIN : {
        if(isAvNextClient()) {
            QByteArray content = dgram.getContent();
            resolveLogin(&content, address, port, dgram.getTimeStamp());
            qDebug() << "Port:" << port;
        }
        break;
    }
    case Datagram::LOGIN_ACK : {
        emit clientConnectionAck(dgram.getClientId());
        break;
    }
    case Datagram::LOGOUT : {
        clientDisconnected(dgram);
        break;
    }
    case Datagram::NEW_CHANNEL : {
        newChannel(dgram, address, port);
        break;
    }
    case Datagram::CLOSE_CHANNEL : {
        closeChannel(dgram);
        break;
    }
    case Datagram::GET_LIST : {
        QByteArray content =  channelModel->serialize();
        Datagram response(Datagram::LIST, 0, dgram.getTimeStamp());
        response.setDatagramContent(&content);
        sendDatagram(address, port, response);
        break;
    }
    case Datagram::SYNCH_RESP : {
        synchResponse(dgram);
        break;
    }
    default : {

        break;
    }
    }
}

bool ManageClients::nextClientId() {
    if(clientModel->rowCount(QModelIndex()) >= MAXCLIENTS) {
        return false;
    }
    if(clientID + 1 >= MAXCLIENTS) {
        clientID = 0;
    }
    else {
        clientID += 1;
    }
    while (clientModel->containsClient(clientID)) {
        if(clientID + 1 >= MAXCLIENTS) {
            clientID = 0;
        }
        else {
            clientID += 1;
        }
    }
    return true;
}

bool ManageClients::isAvNextClient() {
    if(clientModel->rowCount(QModelIndex()) < MAXCLIENTS) {
        return true;
    }
    return false;
}

void ManageClients::resolveLogin(QByteArray *content, QHostAddress address, qint32 port, qint64 timeStamp) {
    //deserialize login data
    qDebug() << address.toString();

    QDataStream out(content, QIODevice::ReadOnly);
    QString osName;
    quint32 clientType;
    out >> clientType;
    out >> osName;
    ClientInfo *info = NULL;
    switch (clientType) {
    case ClientInfo::SPEAKER : {
        info = new SpeakerClientInfo(address, port, osName, 0);
        break;
    }
    case ClientInfo::LISTENER : {
        info = new ListenerClientInfo(address, port, osName, 0);
        break;
    }
    case ClientInfo::TRANSLATOR : {
        info = new TranslatorClientInfo(address, port, osName, 0);
        break;
    }
    default: {
        //illegal client
        return;
        break;
    }
    }
    //clientList.insert(clientID, info);
    Datagram response(Datagram::LOGIN_ACK, 0, timeStamp);
    //insert client id into a buffer
    QByteArray toSend;
    QDataStream in(&toSend, QIODevice::WriteOnly);
    //check if new id can be generated, otherwise return error
    if(nextClientId()) {
        in << clientID;
        info->setId(clientID);
        emit newClientConnected(info);
    }
    else {
        in << 0;
    }
    response.setDatagramContent(&toSend);
    response.sendDatagram(socket, &address, port);
}

void ManageClients::newChannel(const Datagram &dgram, QHostAddress &address, qint32 port) {
    QByteArray content = dgram.getContent();
    ChannelInfo chInfo(content);
    if(nextPort()) {
        chInfo.setOutPort(outPort);
        emit newChannelAdded(chInfo);
        QString toSend("ACK");
        Datagram response(Datagram::NEW_CHANNEL_ACK, 0, dgram.getTimeStamp(), &toSend);
        sendDatagram(address, port, response);

        //send new channel settings to the other clients
        Datagram notify(Datagram::NEW_CHANNEL, 0, dgram.generateTimestamp());
        QByteArray serializedChannel = chInfo.serialize();
        notify.setDatagramContent(&serializedChannel);
        sendCollectiveMessageToListeners(notify);
    }
}

bool ManageClients::nextPort() {
    if(channelModel->rowCount(QModelIndex()) >= MAXCLIENTS) {
        return false;
    }
    if(outPort + 1 >= MAX_PORT) {
        outPort = MIN_PORT;
    }
    else {
        outPort += 1;
    }
    while (channelModel->hasPortAssigned(outPort)) {
        if(outPort + 1 >= MAX_PORT) {
            outPort = MIN_PORT;
        }
        else {
            outPort += 1;
        }
    }
    return true;
}

bool ManageClients::isPortAv() {
    if(channelModel->rowCount(QModelIndex()) < MAXCLIENTS) {
        return true;
    }
    return false;
}

void ManageClients::serverDown() {
    QString toSend("SERVER_DOWN");
    Datagram dgram(Datagram::SERVER_DOWN, 0, Datagram::generateTimestamp(), &toSend);
    sendCollectiveMessage(dgram);
}

void ManageClients::sendCollectiveMessageToSpeakers(Datagram &dgram) {
    QVector< QSharedPointer<ClientInfo> > clients = clientModel->getClientList();
    QVectorIterator< QSharedPointer<ClientInfo> > iter(clients);
    while(iter.hasNext()) {
        QSharedPointer<ClientInfo> client = iter.next();
        if(client.dynamicCast<TranslatorClientInfo>() != NULL) {
            QHostAddress address = client->getAddress();
            qint32 port = client->getClientPort();
            sendDatagram(address, port, dgram);
        }
    }
}

void ManageClients::sendDatagram(QHostAddress &address, qint32 port,  Datagram &dgram) {
    dgram.sendDatagram(socket, &address, port);
}

void ManageClients::sendCollectiveMessageToListeners(Datagram &dgram) {
    QVector< QSharedPointer<ClientInfo> > clients = clientModel->getClientList();
    QVectorIterator< QSharedPointer<ClientInfo> > iter(clients);
    while(iter.hasNext()) {
        QSharedPointer<ClientInfo> client = iter.next();
        if(client.dynamicCast<ListenerClientInfo>() != NULL) {
            QHostAddress address = client->getAddress();
            qint32 port = client->getClientPort();
            sendDatagram(address, port, dgram);
        }
    }
}

void ManageClients::sendCollectiveMessage(Datagram &dgram) {
    QVector< QSharedPointer<ClientInfo> > clients = clientModel->getClientList();
    QVectorIterator< QSharedPointer<ClientInfo> > iter(clients);
    while(iter.hasNext()) {
        QSharedPointer<ClientInfo> client = iter.next();
        QHostAddress address = client->getAddress();
        qint32 port = client->getClientPort();
        sendDatagram(address, port, dgram);
    }
}

void ManageClients::clientDisconnected(const Datagram &dgram) {
    //close his channel if it was a speaker or translator
    closeChannel(dgram);
    //emit signal
    emit clientDisconnectedSignal(dgram.getClientId());
}

void ManageClients::synchronizeClients() {
    clientModel->removeOfflineClients();
    QString toSend("SYNCH");
    Datagram dgram(Datagram::SYNCH, 0, Datagram::generateTimestamp(), &toSend);
    sendCollectiveMessage(dgram);
    qDebug() << "synch size:" << dgram.getSize();
}

void ManageClients::synchResponse(const Datagram &dgram) {
    qDebug() << "synch response";
    QSharedPointer<ClientInfo> client = clientModel->getClientWithId(dgram.getClientId());
    if(!client.isNull()) {
        client->resetNoResponseCounter();
    }
}

void ManageClients::closeChannel(const Datagram &dgram) {
    QPair<bool, int> contains = channelModel->containsChannel(dgram.getClientId());
    if(contains.first) {
        emit channelClosed(dgram.getClientId());
        QByteArray toSend;
        QDataStream in(&toSend, QIODevice::WriteOnly);
        in << dgram.getClientId();
        Datagram dgram(Datagram::REMOVE_CHANNEL, 0, Datagram::generateTimestamp());
        dgram.setDatagramContent(&toSend);
        sendCollectiveMessageToListeners(dgram);
    }
}





