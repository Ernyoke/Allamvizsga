#ifndef DATAGRAM_H
#define DATAGRAM_H

#include <QObject>
#include <QUdpSocket>
#include <QByteArray>
#include <QDebug>
#include <QDateTime>

#include "soundchunk.h"

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
    Datagram(qint32 id, qint32 clientId, qint64 timestamp, SoundChunk *);
    Datagram(qint32 id, qint32 clientId, qint64 timestamp, QString *data);
    Datagram(qint32 id, qint32 clientId, qint64 timestamp);
    Datagram(QByteArray*);
    ~Datagram();

    void setTimeStamp(qint64);
    void setDatagramContent(QByteArray*);
    void sendDatagram(QUdpSocket*, QHostAddress*, int port);
    QByteArray getContent();
    qint32 getSize();
    qint64 getTimeStamp();
    qint32 getId();
    qint32 getClientId();
    qint32 getPacketsNumber();
    qint32 getCurrentPackNummber();

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
    qint32 size;

    void createDatagram(QByteArray *, int packet_nr);
    void splitDatagram();
    void splitContent(QString*);
    void splitContent(QByteArray*);

    qint32 headerSize();



};

#endif // DATAGRAM_H
