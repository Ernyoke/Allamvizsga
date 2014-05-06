#include "acceptdata.h"

AcceptData::AcceptData(int portIn, int portOut, QObject *parent) :
    QThread(parent)
{
    this->portIn = portIn;
    this->portOut = portOut;
    socket = new QUdpSocket(this);
    socket->bind(QHostAddress::Any, portIn);
    groupAddress = QHostAddress("239.255.43.21");
}

void AcceptData::run() {
    connect(socket, SIGNAL(readyRead()), this, SLOT(readData()));
}

void AcceptData::readData() {
    QByteArray data;
    //qDebug() << "Data arrived!";
    data.resize(socket->pendingDatagramSize());
    QHostAddress sender;
    quint16 senderPort;
    socket->readDatagram(data.data(), data.size(), &sender, &senderPort);
    //qDebug() << data;
    socket->writeDatagram(data.data(), data.size(), groupAddress, portOut);
}
