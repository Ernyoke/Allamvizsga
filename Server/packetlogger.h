#ifndef PACKETLOGGER_H
#define PACKETLOGGER_H

#include <QObject>
#include <QFile>
#include <QDataStream>
#include <QMutex>
#include "datagram.h"

class PacketLogger : public QObject
{
    Q_OBJECT
public:
    explicit PacketLogger(QMutex *mutex = 0);
    ~PacketLogger();

protected:
    QMutex *mutex;

    QFile *logFile;
    bool isLogActivated;
    void createLogEntry(Datagram&);

signals:
    void errorMessage(QString);

public slots:
    void startPacketLog(QFile*);
    void stopPacketLog();
};

#endif // PACKETLOGGER_H
