#include "clientinfo.h"

ClientInfo::ClientInfo(QHostAddress address, QString OSName, double version)
{
    this->address = address;
    this->OSName = OSName;
    this->version = version;
    isActive = false;
}


ClientInfo::~ClientInfo()
{

}

void ClientInfo::setId(quint32 id) {
    this->id = id;
}

quint32 ClientInfo::getId() {
    return this->id;
}

QString ClientInfo::getAddressStr() {
    return this->address.toString();
}

QHostAddress ClientInfo::getAddress() {
    return this->address;
}

QString ClientInfo::getOSName() {
    return this->OSName;
}

bool ClientInfo::active() {
    return this->isActive;
}

void ClientInfo::setAck() {
    this->isActive = true;
}
