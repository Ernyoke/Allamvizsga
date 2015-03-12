#include "manageclients.h"
#include "ui_manageclients.h"

const int PORT = 10000;
const int SERVER_PORT = 40000;
const int MAXCLIENTS = 1024;

const int MIN_PORT = 10000;
const int MAX_PORT = 65000;

ManageClients::ManageClients(ClientModel *tableModel, ChannelModel *channelModel, QWidget *parent) :
    QObject(parent)
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
    switch(dgram.getId()) {
    case Datagram::LOGIN : {
        if(isAvNextClient()) {
            QByteArray content = dgram.getContent();
            resolveLogin(&content, address, dgram.getTimeStamp());
        }
        break;
    }
    case Datagram::LOGIN_ACK : {
        emit clientConnectionAck(dgram.getClientId());
        break;
    }
    case Datagram::LOGOUT : {
        emit clientDisconnected(dgram.getClientId());
        break;
    }
    case Datagram::NEW_CHANNEL : {
        newChannel(dgram, address, dgram.getTimeStamp());
        break;
    }
    case Datagram::CLOSE_CHANNEL : {
        emit channelClosed(dgram.getClientId());
        break;
    }
    case Datagram::GET_LIST : {
        QByteArray content =  channelModel->serialize();
        Datagram response(Datagram::LIST, 0, dgram.getTimeStamp());
        response.setDatagramContent(&content);
        response.sendDatagram(socket, &address, SERVER_PORT);
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

void ManageClients::resolveLogin(QByteArray *content, QHostAddress address, qint64 timeStamp) {
    //deserialize login data
    qDebug() << timeStamp;

    QDataStream out(content, QIODevice::ReadOnly);
    QString osName;
    quint32 clientType;
    out >> clientType;
    out >> osName;
    ClientInfo *info = NULL;
    switch (clientType) {
    case ClientInfo::SPEAKER : {
        info = new SpeakerClientInfo(address, osName, 0);
        break;
    }
    case ClientInfo::LISTENER : {
        info = new ListenerClientInfo(address, osName, 0);
        break;
    }
    case ClientInfo::TRANSLATOR : {
        info = new TranslatorClientInfo(address, osName, 0);
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
    response.sendDatagram(socket, &address, SERVER_PORT);
}

void ManageClients::newChannel(Datagram &dgram, QHostAddress &address, qint64 timeStamp) {
    QByteArray content = dgram.getContent();
    ChannelInfo chInfo(content);
    if(nextPort()) {
        chInfo.setOutPort(outPort);
        emit newChannelAdded(chInfo);
        QString toSend("ACK");
        Datagram response(Datagram::NEW_CHANNEL_ACK, 0, dgram.getTimeStamp(), &toSend);
        response.sendDatagram(socket, &address, SERVER_PORT);

        //send new channel settings to the other clients
        Datagram notify(Datagram::NEW_CHANNEL, 0, dgram.generateTimestamp());
        QByteArray serializedChannel = chInfo.serialize();
        notify.setDatagramContent(&serializedChannel);
        sendCollectiveMessage(notify);
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

void ManageClients::sendCollectiveMessageToSpeakers(Datagram &dgram) {
    QVector< QSharedPointer<ClientInfo> > clients = clientModel->getClientList();
    QVectorIterator< QSharedPointer<ClientInfo> > iter(clients);
    while(iter.hasNext()) {
        QSharedPointer<ClientInfo> client = iter.next();
        if(dynamic_cast<SpeakerClientInfo*>(client.data()) == NULL) {
            dgram.sendDatagram(socket, &client->getAddress(), SERVER_PORT);
        }
    }
}

void ManageClients::sendCollectiveMessage(Datagram &dgram) {
    QVector< QSharedPointer<ClientInfo> > clients = clientModel->getClientList();
    QVectorIterator< QSharedPointer<ClientInfo> > iter(clients);
    while(iter.hasNext()) {
        QSharedPointer<ClientInfo> client = iter.next();
        dgram.sendDatagram(socket, &client->getAddress(), SERVER_PORT);
    }
}

void ManageClients::synchronizeClients() {
    clientModel->removeOfflineClients();
    QString toSend("SYNCH");
    Datagram dgram(Datagram::SYNCH, 0, Datagram::generateTimestamp(), &toSend);
    sendCollectiveMessage(dgram);
}

void ManageClients::synchResponse(Datagram &dgram) {
    qDebug() << "synch response";
    QSharedPointer<ClientInfo> client = clientModel->getClientWithId(dgram.getClientId());
    client->resetNoResponseCounter();
}





