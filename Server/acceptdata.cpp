#include "acceptdata.h"

AcceptData::AcceptData(int portIn, int portOut, QObject *parent) :
    QThread(parent)
{
    this->portIn = portIn;
    this->portOut = portOut;
    socket = new QUdpSocket(this);
    socket->bind(QHostAddress::Any, portIn);
}

void AcceptData::run() {
    connect(socket, SIGNAL(readyRead()), this, SLOT(readData()));
}

void AcceptData::readData() {
    QByteArray data;
    data.resize(socket->pendingDatagramSize());
    QHostAddress sender;
    quint16 senderPort;
    socket->readDatagram(data.data(), data.size(), &sender, &senderPort);
    socket->writeDatagram(data.data(), data.size(), QHostAddress::Broadcast, portOut);
}
