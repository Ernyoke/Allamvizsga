#include "servercommunicator.h"

const int SERVER_CLIENTID = 0;
const int ONE_SEC = 10000;
const int FIVE_SEC = 5 * ONE_SEC;

ServerCommunicator::ServerCommunicator(Settings *settings, QObject *parent) : QObject(parent)
{
    this->settings = settings;
    socket = NULL;

    refreshTimer = new QTimer(this);
    connect(refreshTimer, SIGNAL(timeout()), this, SLOT(determineServerStatus()));

    loginTimer = new QTimer(this);
    connect(loginTimer, SIGNAL(timeout()), this, SLOT(loginTimedOut()));

    refreshCounter = 0;

    authentificationStatus = false;
    qDebug() << "constructor";
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
    qDebug() << "authstatus: " << authentificationStatus;
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
        //invalud package, throw it
        break;
    }
    case Datagram::SOUND : {
        //invalid package, sound packages should not arive on this port
        break;
    }
    case Datagram::NEW_CHANNEL : {
        //invalid package, throw it
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
        //invalid package, throw it
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

void ServerCommunicator::sendLoginRequest() {
    if(!authentificationStatus)  {
        //get the current timespamp;
        qint64 timeStamp = Datagram::generateTimestamp();
        //get system information
        QSysInfo sysInfo;
        QString os = sysInfo.prettyProductName();
        //create buffer which needs to be sent
        //4 byte SPEAKER_ID, remaining bytes os info
        QByteArray content;
        QDataStream in(&content, QIODevice::WriteOnly);
        in << settings->getClientType();
        in << os;
        //create the datagram
        Datagram dgram(Datagram::LOGIN, settings->getClientId(), timeStamp);
        dgram.setDatagramContent(&content);
        //recreate the socket
        delete socket;
        socket = new QUdpSocket(this);
//        socket->bind(*settings->getServerAddress(), settings->getClientPort());
        socket->bind(settings->getClientPort());
        connect(socket, SIGNAL(readyRead()), this, SLOT(readDatagram()));
        sendDatagram(&dgram);
        //create timer for login response
        loginTimer->start(5 * ONE_SEC);
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
        Datagram response(Datagram::LOGOUT, settings->getClientId(), Datagram::generateTimestamp(), &respContent);
        sendDatagram(&response);
    }
}

void ServerCommunicator::sendDatagram(Datagram *dgram) {
    dgram->sendDatagram(socket, settings->getServerAddress(), settings->getServerPort());
}

void ServerCommunicator::sendDatagram(Datagram dgram) {
    dgram.sendDatagram(socket, settings->getServerAddress(), settings->getServerPort());
}

void ServerCommunicator::processLogin(Datagram& dgram) {
    if(!authentificationStatus) {
        if(dgram.getId() == Datagram::LOGIN_ACK) {
            QByteArray content(dgram.getContent());
            QDataStream out(&content, QIODevice::ReadOnly);
            qint32 id;
            out >> id;
            if(id > 0) {
                emit authentificationSucces(id);
                QString respContent = " ";
                Datagram response(Datagram::LOGIN_ACK, settings->getClientId(), Datagram::generateTimestamp(), &respContent);
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

//called when the server sends a synchronization packet
void ServerCommunicator::processSynch(Datagram &dgram) {
    if(authentificationStatus) {
        if(dgram.getClientId() == SERVER_CLIENTID) {
            QString toSend("SYNCH");
            Datagram response(Datagram::SYNCH_RESP, settings->getClientId(), Datagram::generateTimestamp(), &toSend);
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


