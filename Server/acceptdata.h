#ifndef ACCEPTDATA_H
#define ACCEPTDATA_H

#include <QObject>
#include <QMap>
#include <QUdpSocket>
#include <QHostAddress>
#include <QMetaType>
#include <QFile>
#include <QDir>
#include "channelinfo.h"
#include "datagram.h"
#include "packetlogger.h"

class AcceptData : public PacketLogger
{
    Q_OBJECT
public:
    AcceptData(QHostAddress, QMutex *mutex);
    ~AcceptData();
    int getPortIn();
    int getPortOut();

private:
    QUdpSocket *socket;
    QHostAddress broadcastAddress;
    QByteArray data;

    QMap<qint32, ChannelInfo*> channels;
    bool isDataAvailable;
    bool isRunning;

signals:
    void finished();
    void errorMessage(QString);

public slots:
    void init();
    void readData();
    void addChannel(ChannelInfo);
    void removeChannel(qint32);
    void stopWorker();
    void changeBroadcastAddress(QString);

};

#endif // ACCEPTDATA_H
