#ifndef DATAGRAM_H
#define DATAGRAM_H

#include <QObject>
#include <QUdpSocket>
#include <QByteArray>
#include <QDebug>
#include <QDateTime>
#include <QMutex>

//#include "soundchunk.h"

//this class defines the protocol which is needed for UDP data transfer

class Datagram
{
public:

    enum PROTOCOL_ID {
        LOGIN = 1,
        LOGIN_ACK = 2,
        LOGOUT = 3,
        GET_LIST = 4,
        LIST = 5,
        SOUND = 6,
        NEW_CHANNEL = 7,
        NEW_CHANNEL_ACK = 8,
        CLOSE_CHANNEL = 9,
        REMOVE_CHANNEL = 10,
        SYNCH = 11,
        SYNCH_RESP = 12,
        SERVER_DOWN = 13
    };

    explicit Datagram();
//    Datagram(quint32 id, quint32 clientId, quint64 timestamp, SoundChunk *);
    Datagram(qint32 id, qint32 clientId, qint64 timestamp, QString *data);
    Datagram(qint32 id, qint32 clientId, qint64 timestamp);
    Datagram(QByteArray*);
    ~Datagram();

    void setTimeStamp(qint64);
    void setDatagramContent(QByteArray*);
    void sendDatagram(QUdpSocket*, QHostAddress*, int port);
    QByteArray getContent() const;
    qint32 getSize() const;
    qint64 getTimeStamp() const;
    qint32 getId() const;
    qint32 getClientId() const;
    qint32 getCurrentPackNumber() const;
    qint32 getPacketsNumber() const;

    //static methods
    static qint64 generateTimestamp();

private:
    qint64 timestamp;
    qint32 id;
    qint32 clientId;
    qint32 packets;
    qint32 packetnr;
    qint32 buffsize;
    //contains the all the data that needs to be sent
    QVector<QByteArray *>data;
    //cointain the data for a single packet
    QByteArray buffer;

    //data size
    quint32 size;

    static qint64 packetCounter;

    void createDatagram(QByteArray *, int packet_nr);
    void splitDatagram();
    void splitContent(QString*);
    void splitContent(QByteArray*);

    quint32 headerSize() const;




};

#endif // DATAGRAM_H
