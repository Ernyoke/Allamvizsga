#include "datagram.h"

const int CONTENTSIZE = 65000;

qint64 Datagram::packetCounter = 0;

Datagram::Datagram()
{
}

Datagram::Datagram(qint32 id, qint32 clientId, qint64 timestamp, SoundChunk *data){
    this->timestamp = timestamp;
    this->id = id;
    this->clientId = clientId;
    QByteArray *serialized = new QByteArray(data->serialize());
    this->data.append(serialized);
    this->size = headerSize() + data->getSize();
}

Datagram::Datagram(QByteArray *data) {
    this->buffer.append(*data);
    splitDatagram();
}

Datagram::Datagram(qint32 id, qint32 clientId, qint64 timestamp, QString *data) {
    this->timestamp = timestamp;
    this->id = id;
    this->clientId = clientId;
    this->splitContent(data);
}

Datagram::Datagram(qint32 id, qint32 clientId, qint64 timestamp) {
    this->timestamp = timestamp;
    this->id = id;
    this->clientId = clientId;
    this->size = headerSize();
}

void Datagram::splitContent(QString *data) {
    QString tempData = *data;
    size = 0;
    packets = 0;
    while(tempData.length() > CONTENTSIZE) {
        QString piece = tempData.left(CONTENTSIZE);
        QByteArray *dataChunk = new QByteArray;
        dataChunk->insert(0, piece);
        this->data.append(dataChunk);
        size += dataChunk->size() + headerSize();
        tempData.remove(0, CONTENTSIZE);
        packets++;
    }
    if(tempData.size() > 0) {
        QString piece = tempData;
        QByteArray *dataChunk = new QByteArray;
        dataChunk->insert(0, piece);
        this->data.append(dataChunk);
        size += dataChunk->size() + headerSize();
        tempData.remove(0, CONTENTSIZE);
        //
        packets++;
    }
}

void Datagram::splitContent(QByteArray *data) {
    QByteArray *tempData = new QByteArray(*data);
    size = 0;
    packets = 0;
    while(tempData->length() > CONTENTSIZE) {
        QByteArray *piece = new QByteArray(tempData->left(CONTENTSIZE));
        this->data.append(piece);
        tempData->remove(0, CONTENTSIZE);
        packets++;
        size += piece->size() + headerSize();
    }
    if(tempData->size() > 0) {
        this->data.append(tempData);
        size += tempData->size() + headerSize();
        packets++;
    }
}


Datagram::~Datagram() {
//    delete data;
}

void Datagram::setTimeStamp(qint64 timestamp) {
    this->timestamp = timestamp;
}

void Datagram::setDatagramContent(QByteArray *data) {
    this->splitContent(data);
}

void Datagram::sendDatagram(QUdpSocket *socket, QHostAddress *IPAddress, int port) {
    int i = 0;
    for(QVector<QByteArray*>::iterator it = data.begin(); it != data.end(); ++it) {
        createDatagram(*it, i);
        socket->writeDatagram(buffer, *IPAddress, port);
        ++i;
    }
}

void Datagram::splitDatagram() {
    QDataStream in(&buffer, QIODevice::ReadOnly);
    in >> recPacketCounter;
    in >> id;
    in >> clientId;
    in >> timestamp;
    in >> packets;
    in >> packetnr;
    QByteArray *temp = new QByteArray;
    in >> *temp;
    data.append(temp);
}


void Datagram::createDatagram(QByteArray *dataToSend, int packet_nr) {
    QDataStream out(&buffer, QIODevice::WriteOnly);

    QMutex mutex;

    mutex.lock();
    packetCounter++;
    out << packetCounter;
    mutex.unlock();

    out << id;
    out << clientId;
    out << timestamp;
    out << packets;
    out << packet_nr;
    out << *dataToSend;
}

qint32 Datagram::getSize() const {
    return this->size;
}

QByteArray Datagram::getContent() const {
    QByteArray content;
    QVectorIterator<QByteArray*> it(data);
    while(it.hasNext()) {
        content.append(*it.next());
    }
    return content;
}

qint64 Datagram::getTimeStamp() const {
    return timestamp;
}

qint32 Datagram::getId() const {
    return this->id;
}

quint32 Datagram::headerSize() const {
    quint32 hsize = sizeof(timestamp) + sizeof(id) + sizeof(clientId) + sizeof(packets) + sizeof(packetnr) + sizeof(buffsize);
    return hsize;
}

qint32 Datagram::getClientId() const {
    return this->clientId;
}

qint32 Datagram::getCurrentPackNumber() const {
    return this->packetnr;
}

qint32 Datagram::getPacketsNumber() const {
    return this->packets;
}

qint64 Datagram::generateTimestamp() {
    QDateTime now = QDateTime::currentDateTime();
    qint64 timeStamp = now.currentDateTime().toMSecsSinceEpoch();
    return timeStamp;
}

qint64 Datagram::getPacketCounter() const {
    return recPacketCounter;
}
