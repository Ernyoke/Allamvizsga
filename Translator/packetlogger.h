#ifndef PACKETLOGGER_H
#define PACKETLOGGER_H

#include <QFile>
#include <QDataStream>
#include <QMutex>
#include "datagram.h"

class PacketLogger
{
public:
    PacketLogger();
    ~PacketLogger();

    enum DIRECTION { IN, OUT };

    void createLogEntry(DIRECTION, Datagram&);

    void startPacketLog(QMutex *mutex, QFile *logFile);
    void stopPacketLog();

protected:
    QMutex *mutex;

    QFile *logFile;
    bool isLogActivated;

};

#endif // PACKETLOGGER_H

