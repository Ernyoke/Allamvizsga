#include "manageclients.h"

const int PORT = 10000;
const int MAXCLIENTS = 1024;

ManageClients::ManageClients()
{
    this->socket = new QUdpSocket(this);
    socket->bind(PORT);

    clientID = 0;

    connect(socket, SIGNAL(readyRead()), this, SLOT(readPendingDatagrams()));
}

ManageClients::~ManageClients()
{
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
        if(nextClientId()) {
            QByteArray *content = dgram.getContent();
            //deserialize login data
            QDataStream out(content, QIODevice::ReadOnly);
            QString osName;
            uint32_t clientType;
            out >> clientType;
            out >> osName;
            ClientInfo *info =  new ClientInfo(address, port, clientType, osName, 0);
            clientList.insert(clientID, info);
            Datagram response(Datagram::LOGIN_ACK, 0, dgram.getTimeStamp());
            //insert client id into a buffer
            QByteArray toSend;
            QDataStream in(&toSend, QIODevice::WriteOnly);
            //check if new id can be generated, otherwise return error
            if(nextClientId()) {
                in << clientID;
            }
            else {
                in << 0;
            }
            response.setDatagramContent(&toSend);
            response.sendDatagram(socket, &address, 40000);
        }
        break;
    }
    case Datagram::LOGIN_ACK : {

        break;
    }
    case Datagram::LOGOUT : {

        break;
    }
    default : {

        break;
    }
    }
}

bool ManageClients::nextClientId() {
    if(clientList.size() >= MAXCLIENTS) {
        return false;
    }
    if(clientID + 1 >= MAXCLIENTS) {
        clientID = 0;
    }
    else {
        clientID += 1;
    }
    while (clientList.contains(clientID)) {
        if(clientID + 1 >= MAXCLIENTS) {
            clientID = 0;
        }
        else {
            clientID += 1;
        }
    }
    return true;

}


