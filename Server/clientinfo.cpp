#include "clientinfo.h"

ClientInfo::ClientInfo(QHostAddress address, qint32 port, QString OSName, double version)
{
    this->address = address;
    this->OSName = OSName;
    this->version = version;
    this->clientPort = port;
    isActive = false;
    noResponseCounter = 0;
}


ClientInfo::~ClientInfo()
{

}

void ClientInfo::setId(quint32 id) {
    this->id = id;
}

quint32 ClientInfo::getId() const {
    return this->id;
}

QString ClientInfo::getAddressStr() const {
    return this->address.toString();
}

QHostAddress ClientInfo::getAddress() const {
    return this->address;
}

qint32 ClientInfo::getClientPort() const {
    return this->clientPort;
}

QString ClientInfo::getOSName() const {
    return this->OSName;
}

bool ClientInfo::active() const {
    return this->isActive;
}

void ClientInfo::setAck() {
    this->isActive = true;
}

void ClientInfo::incNoResponseCounter() {
    this->noResponseCounter++;
}

bool ClientInfo::isOnline() const {
    if(noResponseCounter >= 5) {
        return false;
    }
    return true;
}

void ClientInfo::resetNoResponseCounter() {
    this->noResponseCounter = 0;
}


