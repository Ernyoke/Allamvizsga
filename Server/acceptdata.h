#ifndef ACCEPTDATA_H
#define ACCEPTDATA_H

#include <QObject>
#include <QMap>
#include <QUdpSocket>
#include <QHostAddress>
#include "channelinfo.h"
#include "datagram.h"

class AcceptData : public QObject
{
    Q_OBJECT
public:
    explicit AcceptData(QObject *parent = 0);
    ~AcceptData();
    int getPortIn();
    int getPortOut();

private:
    QUdpSocket *socket;
    QHostAddress groupAddress;
    QByteArray data;

    QMap<qint32, ChannelInfo*> channels;
    bool isDataAvailable;
    bool isRunning;

signals:
    void finished();

public slots:
    void readData();
    void addChannel(ChannelInfo);
    void removeChannel(qint32);
    void stopWorker();

};

#endif // ACCEPTDATA_H
