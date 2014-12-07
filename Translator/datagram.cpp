#include "datagram.h"

Datagram::Datagram()
{
}

Datagram::Datagram(uint32_t id, uint64_t timestamp, SoundChunk *data){
    this->timestamp = timestamp;
    this->id = id;
    this->data = new QByteArray;
    this->data = data->serialize();
}

Datagram::Datagram(QByteArray *data) {
    this->buffer.append(*data);
    splitDatagram();
}

Datagram::Datagram(uint32_t id, uint64_t timestamp, QString *data) {
    this->timestamp = timestamp;
    this->id = id;
    this->data = new QByteArray;
    this->data->append(*(data->data()));
}

Datagram::~Datagram() {
    delete data;
}

void Datagram::setTimeStamp(qint64 timestamp) {
    this->timestamp = timestamp;
}

void Datagram::setDatagram(QByteArray *buff) {
    this->buffer.append(*buff);
}

void Datagram::sendDatagram(QUdpSocket *socket, QHostAddress *IPAddress, int port) {
    createDatagram();
    socket->writeDatagram(buffer, *IPAddress, port);
    qDebug() << buffer.size();
}

void Datagram::splitDatagram() {
    QDataStream in(&buffer, QIODevice::ReadOnly);
    int size;
    in >> id;
    in >> timestamp;
    in >> packets;
    in >> packetnr;
    in >> size;
    int headersize = sizeof(id) + sizeof(timestamp) + sizeof(packets) + sizeof(packetnr) + sizeof(size);
    QByteArray temp(buffer.mid(headersize, size + headersize));
    data = new QByteArray(temp);
}


void Datagram::createDatagram() {
    QDataStream out(&buffer, QIODevice::WriteOnly);
    out << id;
    out << timestamp;
    out << packets;
    out << packetnr;
    out << data->size();
    buffer.append(*data);
}

int Datagram::getSize() {
    return buffer.size();
}

QByteArray* Datagram::getContent() {
    QByteArray *content = new QByteArray(*data);
    return content;
}

qint64 Datagram::getTimeStamp() {
    return timestamp;
}
