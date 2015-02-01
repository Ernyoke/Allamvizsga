#ifndef DATAGRAM_H
#define DATAGRAM_H

#include <QObject>
#include <QUdpSocket>
#include <QByteArray>
#include <cstdint>

//this class defines the protocol which is needed for UDP data transfer

class Datagram
{
public:

    enum PROTOCOL_ID {
        LOGIN = 1,
        LOGIN_ACK = 2,
        LOGOUT = 3,
        GET_LIST = 4,
        SOUND = 5
    };

    explicit Datagram();
//    Datagram(uint32_t id, uint32_t clientId, uint64_t, SoundChunk *);
    Datagram(quint32 id, quint32 clientId, quint32 timestamp, QString *);
    Datagram(quint32 id, quint32 clientId, quint64 timestamp);
    Datagram(QByteArray*);
    ~Datagram();

    void setTimeStamp(qint64);
    void setDatagramContent(QByteArray*);
    void sendDatagram(QUdpSocket*, QHostAddress*, int port);
    QByteArray* getContent();
    int getSize();
    qint64 getTimeStamp();
    int getId();

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
    int size;

    void createDatagram(QByteArray *, int packet_nr);
    void splitDatagram();
    void splitContent(QString*);
    void splitContent(QByteArray*);



};

#endif // DATAGRAM_H
