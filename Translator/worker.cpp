#include "worker.h"

Worker::Worker()
{
    packetLogger = new PacketLogger;
}

Worker::~Worker()
{

}

void Worker::initPacketLog() {
    packetLogger = new PacketLogger;
}

void Worker::startPacketLog(QMutex *mutex, QFile *file) {
    packetLogger->startPacketLog(mutex, file);
}

void Worker::stopPacketLog() {
    packetLogger->stopPacketLog();
}

void Worker::createLogEntry(PacketLogger::DIRECTION dir, Datagram &dgram) {
    packetLogger->createLogEntry(dir, dgram);
}
