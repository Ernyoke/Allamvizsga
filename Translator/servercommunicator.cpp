#include "servercommunicator.h"

const int SERVER_CLIENTID = 0;
const int ONE_SEC = 10000;
const int FIVE_SEC = 5 * ONE_SEC;

ServerCommunicator::ServerCommunicator(QObject *parent) : QObject(parent)
{
    socket = NULL;
    listReqTimer = new QTimer(this);
    connect(listReqTimer, SIGNAL(timeout()), this, SLOT(listReqTimedOut()));

    refreshTimer = new QTimer(this);
    connect(refreshTimer, SIGNAL(timeout()), this, SLOT(determineServerStatus()));

    loginTimer = new QTimer(this);
    connect(loginTimer, SIGNAL(timeout()), this, SLOT(loginTimedOut()));

    reqListStart = 0;
    refreshCounter = 0;

    authentificationStatus = false;
    qDebug() << "constructor";

    serverPort = 10000;
    clientPort = 40000;
    clientPortForSound = 20000;

}

ServerCommunicator::~ServerCommunicator()
{

}


void ServerCommunicator::readDatagram() {

    while(socket->hasPendingDatagrams()) {
        QByteArray dataReceived;
        dataReceived.resize(socket->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;
        socket->readDatagram(dataReceived.data(), dataReceived.size(), &sender, &senderPort);
        Datagram dgram(&dataReceived);
        this->processDatagram(dgram);
    }
}

void ServerCommunicator::processDatagram(Datagram &dgram) {
    createLogEntry(DIRECTION::IN, dgram);
    switch(dgram.getId()) {
    case Datagram::LOGIN :{
        //invalid package, throw it
        break;
    }
    case Datagram::LOGIN_ACK :{
        //emit login signal for logindialog
        processLogin(dgram);
        break;
    }
    case Datagram::LOGOUT : {
        //invalid package, throw it
        break;
    }
    case Datagram::GET_LIST : {
        //invalid package, throw it
        break;
    }
    case Datagram::LIST : {
        //process list response, waitig for multiple packages
        processList(dgram);
        break;
    }
    case Datagram::SOUND : {
        //invalid package, sound packages should not arive on this port
        break;
    }
    case Datagram::NEW_CHANNEL : {
        //invalid package, throw it
        processNewChannel(dgram);
        break;
    }
    case Datagram::NEW_CHANNEL_ACK : {
        //emit new chanel signal
        emit newChannelAckReceived(dgram);
        break;
    }
    case Datagram::CLOSE_CHANNEL : {
        //invalid package, throw it
        break;
    }
    case Datagram::REMOVE_CHANNEL : {
        processRemoveChannel(dgram);
        break;
    }
    case Datagram::SYNCH : {
        processSynch(dgram);
        break;
    }
    case Datagram::SYNCH_RESP : {
        //invalid package, throw it
        break;
    }
    case Datagram::SERVER_DOWN : {
        //server exit
        processServerDown(dgram);
        break;
    }
    }
}

void ServerCommunicator::sendLoginRequest(QString address) {
    if(!authentificationStatus)  {
        if(Settings::checkIpAddress(address)) {
            serverAddress = QHostAddress(address);

        }
        else {
            emit authentificationFailed();
            return;
        }
        //get the current timespamp;
        qint64 timeStamp = Datagram::generateTimestamp();
        //get system information
        QSysInfo sysInfo;
        QString os = sysInfo.prettyProductName();
        //create buffer which needs to be sent
        //4 byte SPEAKER_ID, remaining bytes os info
        QByteArray content;
        QDataStream in(&content, QIODevice::WriteOnly);
        in << Settings::CLIENT_TYPE;
        in << os;
        //create the datagram
        Datagram dgram(Datagram::LOGIN, 0, timeStamp);
        dgram.setDatagramContent(&content);
        //recreate the socket
        delete socket;
        socket = new QUdpSocket(this);
        //bind to a port
        bool binded = false;
        int tries = 0;
        if(!socket->bind(clientPort)) {
            clientPort++;
            while(tries < 10) {
                if(socket->bind(clientPort)) {
                    binded = true;
                    break;
                }
                clientPort++;
            }
        }
        else {
            binded = true;
        }
        if(!binded) {
            emit authentificationFailed();
            return;
        }
        connect(socket, SIGNAL(readyRead()), this, SLOT(readDatagram()));
        sendDatagram(&dgram);
        //create timer for login response
        loginTimer->start(ONE_SEC);
        loginTimer->setSingleShot(true);
    }
}

void ServerCommunicator::loginTimedOut() {
    if(!authentificationStatus) {
        authentificationStatus = false;
        emit authentificationTimedOut();
    }
}

void ServerCommunicator::logout() {
    if(authentificationStatus) {
        QString respContent = " ";
        Datagram response(Datagram::LOGOUT, myclientId, Datagram::generateTimestamp(), &respContent);
        sendDatagram(&response);
    }
}

void ServerCommunicator::sendDatagram(Datagram *dgram) {
    dgram->sendDatagram(socket, &serverAddress, serverPort);
    createLogEntry(OUT, *dgram);
}

void ServerCommunicator::sendDatagram(Datagram dgram) {
    dgram.sendDatagram(socket, &serverAddress, serverPort);
    createLogEntry(OUT, dgram);
}

void ServerCommunicator::requestChannelList() {
    qDebug() << "reqChannelList";
    if(authentificationStatus) {
        reqListStart = Datagram::generateTimestamp();
        listContent.clear();
        QString toSend("LIST");
        Datagram dgram(Datagram::GET_LIST, myclientId, reqListStart, &toSend);
        sendDatagram(&dgram);
        listReqTimer->setSingleShot(true);
        listReqTimer->start(ONE_SEC);
    }

}

void ServerCommunicator::processLogin(Datagram& dgram) {
    if(!authentificationStatus) {
        if(dgram.getId() == Datagram::LOGIN_ACK) {
            QByteArray content(dgram.getContent());
            QDataStream out(&content, QIODevice::ReadOnly);
            out >> myclientId;
            if(myclientId > 0) {
                emit authentificationSucces(myclientId);
                QString respContent = " ";
                Datagram response(Datagram::LOGIN_ACK, myclientId, Datagram::generateTimestamp(), &respContent);
                sendDatagram(&response);

                authentificationStatus = true;
                socket->flush();

                refreshTimer->start(FIVE_SEC);
                loginTimer->stop();
            }
            else {
                emit authentificationFailed();
                authentificationStatus = true;
            }
        }
    }
}

void ServerCommunicator::processList(Datagram &dgram) {
    if(authentificationStatus) {
        if(dgram.getTimeStamp() == reqListStart) {
            listContent.insert(dgram.getCurrentPackNumber(), dgram.getContent());
            //check if all packets arrived
            if(listContent.size() == dgram.getPacketsNumber()) {
                qDebug() << "list refreshed";
                listReqTimer->stop();
                QByteArray listBuffer;
                QMapIterator<qint32, QByteArray> iter(listContent);
                while(iter.hasNext()) {
                    iter.next();
                    listBuffer.append(iter.value());
                    //deserialize and create list
                    emit serverList(listBuffer);
                }
            }
        }
    }
}

void ServerCommunicator::processNewChannel(Datagram &dgram) {
    if(authentificationStatus) {
        QByteArray content = dgram.getContent();
        ChannelInfo chInfo(content);
        emit channelConnected(chInfo);
    }
}

void ServerCommunicator::listReqTimedOut() {
    reqListStart = 0;
    qDebug() << "list req timedout";
    //error, list can not be created
}

//called when the server sends a synchronization packet
void ServerCommunicator::processSynch(Datagram &dgram) {
    if(authentificationStatus) {
        if(dgram.getClientId() == SERVER_CLIENTID) {
            QString toSend("SYNCH");
            Datagram response(Datagram::SYNCH_RESP, myclientId, Datagram::generateTimestamp(), &toSend);
            sendDatagram(&response);
            refreshCounter++;
            qDebug() << "synch";
        }
    }
}

void ServerCommunicator::determineServerStatus() {
    if(refreshCounter == 0) {
        emit serverDown();
//        authentificationStatus = false;
        refreshTimer->stop();
    }
}

//is called when server goes offline and sends a SERVER_DOWN packet
void ServerCommunicator::processServerDown(Datagram &dgram) {
    if(dgram.getId() == Datagram::SERVER_DOWN) {
        disconnect(socket, SIGNAL(readyRead()), this, SLOT(readDatagram()));
        authentificationStatus = false;
        socket->flush();
        emit serverDown();
    }
}

void ServerCommunicator::processRemoveChannel(Datagram &dgram) {
    if(authentificationStatus) {
        if(dgram.getClientId() == SERVER_CLIENTID) {
            QByteArray content(dgram.getContent());
            QDataStream out(&content, QIODevice::ReadOnly);
            qint32 id;
            out >> id;
            emit removeChannel(id);
        }
    }
}

QHostAddress ServerCommunicator::getServerAddress() const {
    return serverAddress;
}

qint32 ServerCommunicator::getClientId() const {
    return myclientId;
}

qint32 ServerCommunicator::getClientPort() const {
    return clientPort;
}

qint32 ServerCommunicator::getServerPort() const {
    return serverPort;
}

qint32 ServerCommunicator::getClientPortForSound() const {
    return clientPortForSound;
}

//redirect slots for packetlogger
void ServerCommunicator::startPacketLog(QMutex *mutex, QFile *file) {
    PacketLogger::startPacketLog(mutex, file);
}

void ServerCommunicator::stopPacketLog() {
    PacketLogger::stopPacketLog();
}
