#include "servercommunicator.h"

ServerCommunicator::ServerCommunicator(Settings *settings, QObject *parent) : QObject(parent)
{
    this->settings = settings;
    socket = new QUdpSocket(this);
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
    case Datagram::SYNCH : {
        processSynch(dgram);
        break;
    }
    case Datagram::SYNCH_RESP : {
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

void ServerCommunicator::processSynch(Datagram &dgram) {
    QString toSend("SYNCH");
    Datagram response(Datagram::SYNCH_RESP, settings->getClientId(), Datagram::generateTimestamp(), &toSend);
    response.sendDatagram(socket, settings->getServerAddress(), settings->getServerPort());
}


