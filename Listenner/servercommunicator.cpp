#include "servercommunicator.h"

ServerCommunicator::ServerCommunicator(Settings *settings, QObject *parent) : QObject(parent)
{
    this->settings = settings;
    socket = new QUdpSocket(this);
    listReqTimer = new QTimer(this);
    connect(listReqTimer, SIGNAL(timeout()), this, SLOT(listReqTimedOut()));

    reqListStart = 0;
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

void ServerCommunicator::processDatagram(Datagram dgram) {
    switch(dgram.getId()) {
    case Datagram::LOGIN :{
        //invalid package, throw it
        break;
    }
    case Datagram::LOGIN_ACK :{
        //emit login signal for logindialog
        emit loginAckReceived(dgram);
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
        break;
    }
    case Datagram::NEW_CHANNEL_ACK : {
        //emit new chanel signal
//        emit newChannelAckReceived(dgram);
        break;
    }
    case Datagram::CLOSE_CHANNEL : {
        //invalid package, throw it
        break;
    }
    }
}

void ServerCommunicator::sendLoginRequest(Datagram dgram) {
    socket->bind(*settings->getServerAddress(), settings->getClientPort());
    connect(socket, SIGNAL(readyRead()), this, SLOT(readDatagram()));
    sendDatagram(dgram);
}

void ServerCommunicator::sendDatagram(Datagram dgram) {
    dgram.sendDatagram(socket, settings->getServerAddress(), settings->getServerPort());
}

void ServerCommunicator::requestChannelList() {
    reqListStart = Datagram::generateTimestamp();
    listContent.clear();
    QString toSend("LIST");
    Datagram dgram(Datagram::GET_LIST, settings->getClientId(), reqListStart, &toSend);
    dgram.sendDatagram(socket, settings->getServerAddress(), settings->getServerPort());
    listReqTimer->setSingleShot(true);
    listReqTimer->start(10000);

}

void ServerCommunicator::processList(Datagram &dgram) {
    if(dgram.getTimeStamp() == reqListStart) {
        listContent.insert(dgram.getCurrentPackNummber(), dgram.getContent());
        //check if all packets arrived
        if(listContent.size() == dgram.getPacketsNumber()) {
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

void ServerCommunicator::listReqTimedOut() {
    reqListStart = 0;
    //error, list can not be created
}
