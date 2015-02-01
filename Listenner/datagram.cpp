#include "datagram.h"

const int CONTENTSIZE = 65000;

Datagram::Datagram()
{
}

Datagram::Datagram(quint32 id, quint32 clientId, quint64 timestamp, SoundChunk *data){
    this->timestamp = timestamp;
    this->id = id;
    this->clientId = clientId;
    //this->data = new QVector<QByteArray*>();

//    this->size = data->g
}

Datagram::Datagram(QByteArray *data) {
    this->buffer.append(*data);
    splitDatagram();
}

Datagram::Datagram(quint32 id, quint32 clientId, quint64 timestamp, QString *data) {
    this->timestamp = timestamp;
    this->id = id;
    this->clientId = clientId;
    this->splitContent(data);
}

Datagram::Datagram(quint32 id, quint32 clientId, quint64 timestamp) {
    this->timestamp = timestamp;
    this->id = id;
    this->clientId = clientId;
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
        size += dataChunk->size();
        tempData.remove(0, CONTENTSIZE);
        packets++;
    }
    if(tempData.size() > 0) {
        QString piece = tempData;
        QByteArray *dataChunk = new QByteArray;
        dataChunk->insert(0, piece);
        this->data.append(dataChunk);
        size += dataChunk->size();
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
        size += piece->size();
    }
    if(tempData->size() > 0) {
        this->data.append(tempData);
        size += tempData->size();
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
    qDebug() << buffer.size();
}

void Datagram::splitDatagram() {
    QDataStream in(&buffer, QIODevice::ReadOnly);
    int size;
    in >> id;
    in >> clientId;
    in >> timestamp;
    in >> packets;
    in >> packetnr;
    in >> size;
    QByteArray *temp = new QByteArray;
    in >> *temp;
    data.append(temp);
}


void Datagram::createDatagram(QByteArray *dataToSend, int packet_nr) {
    QDataStream out(&buffer, QIODevice::WriteOnly);
    out << id;
    out << clientId;
    out << timestamp;
    out << packets;
    out << packet_nr;
    out << dataToSend->size();
    out << *dataToSend;
}

int Datagram::getSize() {
    return this->size;
}

QByteArray* Datagram::getContent() {
    QByteArray *content = new QByteArray;
    QVectorIterator<QByteArray*> it(data);
    while(it.hasNext()) {
        content->append(*it.next());
    }
    return content;
}

qint64 Datagram::getTimeStamp() {
    return timestamp;
}

int Datagram::getId() {
    return this->id;
}
