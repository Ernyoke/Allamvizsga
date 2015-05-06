#ifndef PACKETLOGGER_H
#define PACKETLOGGER_H

#include <QFile>
#include <QDataStream>
#include <QMutex>
#include "datagram.h"

class PacketLogger
{
public:
    explicit PacketLogger();
    ~PacketLogger();

protected:
    QMutex *mutex;

    QFile *logFile;
    bool isLogActivated;
    void createLogEntry(Datagram&);

    void startPacketLog(QMutex *mutex, QFile *logFile);
    void stopPacketLog();
};

#endif // PACKETLOGGER_H

