#include "connecttoserver.h"

connectToServer::connectToServer(QUdpSocket *socket, GUI *gui, QObject *parent) :
    QObject(parent)
{
    this->socket = socket;
    this->gui = gui;
    sendBuffer = new QByteArray(this);
}

bool connectToServer::conn() {
    int head = 1;
    sendBuffer->append((char)head);
    socket->writeDatagram(sendBuffer, QHostAddress::LocalHost, 20000);
    if(socket->waitForReadyRead(3000)) {
        //gui
        QByteArray receiveBuffer;
        socket->readDatagram(receiveBuffer, QHostAddress::LocalHost, 20000);
        QByteArray id = receiveBuffer.mid(0, 4);
        this->threadID = id.toInt();
        return true;
    }
    return false;
}

int connectToServer::getID() {
    return this->threadID;
}

