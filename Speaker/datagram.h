#ifndef DATAGRAM_H
#define DATAGRAM_H

#include <QObject>
#include <QUdpSocket>
#include <QByteArray>
#include <cstdint>

#include "soundchunk.h"

//this class defines the protocol which is needed for UDP data transfer

class Datagram
{
public:
    explicit Datagram();
    Datagram(uint32_t id, uint64_t, SoundChunk *);
    Datagram(uint32_t id, uint64_t, QString *);
    Datagram(QByteArray*);
    ~Datagram();

    void setTimeStamp(qint64);
    void setDatagram(QByteArray*);
    void sendDatagram(QUdpSocket*, QHostAddress*, int port);
    QByteArray* getContent();
    int getSize();
    qint64 getTimeStamp();

private:
    uint64_t timestamp;
    uint32_t id;
    uint32_t packets;
    uint32_t packetnr;
    uint32_t buffsize;
    QByteArray *data;
    QByteArray buffer;

    void createDatagram();
    void splitDatagram();

};

#endif // DATAGRAM_H
