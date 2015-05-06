#include "packetlogger.h"

PacketLogger::PacketLogger()
{
    isLogActivated = false;
}

PacketLogger::~PacketLogger()
{

}

void PacketLogger::startPacketLog(QMutex *mutex, QFile *logFile) {
    if(!isLogActivated) {
         isLogActivated = true;
         this->mutex = mutex;
         this->logFile = logFile;
    }
}

void PacketLogger::stopPacketLog() {
    if(isLogActivated) {
        isLogActivated = false;
    }
}

void PacketLogger::createLogEntry(Datagram &dgram) {
    if(isLogActivated) {
        QTextStream stream(logFile);
        mutex->lock();
        QString type("INVALID");
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
        stream << dgram.getClientId() << " " << type << " " << dgram.getPacketCounter() << " "
               << dgram.getTimeStamp() << " " << Datagram::generateTimestamp() << "\n";
        mutex->unlock();
    }
}

