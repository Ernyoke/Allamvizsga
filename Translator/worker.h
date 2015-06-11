#ifndef WORKER_H
#define WORKER_H

#include <QObject>
#include <QUdpSocket>
#include <QAudioDeviceInfo>
#include <QAudioFormat>
#include <QPointer>
#include <QFile>
#include <QMutex>

#include "soundchunk.h"
#include "packetlogger.h"

class Worker : public QObject
{
    Q_OBJECT
public:
    Worker();

    virtual ~Worker();

protected:

    QHostAddress serverAddress;
    QPointer<QUdpSocket> socket;
    QAudioFormat format;
    PacketLogger *packetLogger;

signals:
    void errorMessage(QString);
    void finished();

public slots:
    void initPacketLog();
    void startPacketLog(QMutex*, QFile*);
    void stopPacketLog();
    void createLogEntry(PacketLogger::DIRECTION, Datagram&);

};

#endif // WORKER_H
