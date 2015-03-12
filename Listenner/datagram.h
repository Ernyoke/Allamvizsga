#ifndef DATAGRAM_H
#define DATAGRAM_H

#include <QObject>
#include <QUdpSocket>
#include <QByteArray>
#include <QDateTime>
#include <QDebug>

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
        SYNCH = 10,
        SYNCH_RESP = 11
    };

    explicit Datagram();
    Datagram(quint32 id, quint32 clientId, quint64 timestamp, SoundChunk *);
    Datagram(quint32 id, quint32 clientId, quint64 timestamp, QString *data);
    Datagram(quint32 id, quint32 clientId, quint64 timestamp);
    Datagram(QByteArray*);
    ~Datagram();

    void setTimeStamp(qint64);
    void setDatagramContent(QByteArray*);
    void sendDatagram(QUdpSocket*, QHostAddress*, int port);
    QByteArray getContent();
    quint32 getSize();
    qint64 getTimeStamp();
    quint32 getId();
    quint32 getClientId();
    quint32 getPacketsNumber();
    quint32 getCurrentPackNummber();

    //static methods
    static qint64 generateTimestamp();

private:
    quint64 timestamp;
    quint32 id;
    quint32 clientId;
    quint32 packets;
    quint32 packetnr;
    quint32 buffsize;
    //contains the all the data that needs to be sent
    QVector<QByteArray *>data;
    //cointain the data for a single packet
    QByteArray buffer;


    //data size
    quint32 size;

    void createDatagram(QByteArray *, int packet_nr);
    void splitDatagram();
    void splitContent(QString*);
    void splitContent(QByteArray*);

    quint32 headerSize();



};

#endif // DATAGRAM_H
