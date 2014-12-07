#include "acceptdata.h"

AcceptData::AcceptData(int portIn, int portOut, QObject *parent) :
    QObject(parent)
{
    this->portIn = portIn;
    this->portOut = portOut;
    socket = new QUdpSocket(this);
    socket->bind(QHostAddress::Any, portIn);

    connect(socket, SIGNAL(readyRead()), this, SLOT(readData()));
}

AcceptData::~AcceptData() {
}

void AcceptData::readData() {
    data.resize(socket->pendingDatagramSize());
    QHostAddress sender;
    quint16 senderPort;
    socket->readDatagram(data.data(), data.size(), &sender, &senderPort);
    socket->writeDatagram(data.data(), data.size(), QHostAddress::Broadcast /*QHostAddress("192.168.0.255")*/, portOut);
    qDebug() << portOut;
}

int AcceptData::getPortIn() {
    return this->portIn;
}

int AcceptData::getPortOut() {
    return this->portOut;
}
