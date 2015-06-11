#include "packetlogger.h"

PacketLogger::PacketLogger(QMutex *mutex) : QObject(NULL)
{
    this->mutex = mutex;
    isLogActivated = false;
}

PacketLogger::~PacketLogger()
{

}

void PacketLogger::startPacketLog(QFile *logFile) {
    if(!isLogActivated) {
        this->logFile = logFile;
         isLogActivated = true;
    }
    else {
        //
    }
}

void PacketLogger::stopPacketLog() {
    if(isLogActivated) {
        isLogActivated = false;
    }
}

void PacketLogger::createLogEntry(DIRECTION direction, Datagram &dgram) {
    if(isLogActivated) {
        QTextStream stream(logFile);
        mutex->lock();
        QString type("INVALID");
        QString directionStr("IN");
        switch (dgram.getId()) {
        case Datagram::LOGIN :{
            type = "LOGIN";
            break;
        }
        case Datagram::LOGIN_ACK :{
            type = "LOGIN_ACK";
            break;
        }
        case Datagram::LOGOUT : {
            type = "LOGOUT";
            break;
        }
        case Datagram::GET_LIST : {
            type = "GET_LIST";
            break;
        }
        case Datagram::LIST : {
            type = "LIST";
            break;
        }
        case Datagram::SOUND : {
            type = "SOUND";
            break;
        }
        case Datagram::NEW_CHANNEL : {
            type = "NEW_CHANNEL";
            break;
        }
        case Datagram::NEW_CHANNEL_ACK : {
            type = "NEW_CHANNEL_ACK";
            break;
        }
        case Datagram::CLOSE_CHANNEL : {
            type = "CLOSE_CHANNEL";
            break;
        }
        case Datagram::REMOVE_CHANNEL : {
            type = "REMOVE_CHANNEL";
            break;
        }
        case Datagram::SYNCH : {
            type = "SYNCH";
            break;
        }
        case Datagram::SYNCH_RESP : {
            type = "SYNCH_RESP";
            break;
        }
        case Datagram::SERVER_DOWN : {
            type = "SERVER_DOWN";
            break;
        }
        }

        qint64 counter = 0;
        switch(direction) {
        case IN: {
            directionStr = "IN";
            counter = dgram.getRecPacketCounter();
            break;
        }
        case OUT: {
            directionStr = "OUT";
            counter = dgram.getPacketCounter();
            break;
        }
        }

        stream << directionStr <<  " " << dgram.getClientId() << " " << type << " " << counter << " "
               << dgram.getTimeStamp() << " " << Datagram::generateTimestamp() << " " << dgram.getSize() << "\n";
        mutex->unlock();
    }
}

