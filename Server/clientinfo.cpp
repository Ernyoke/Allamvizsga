#include "clientinfo.h"

ClientInfo::ClientInfo(QHostAddress address, quint16 port, quint32 clientType, QString OSName, double version)
{
    this->address = address;
    this->port = port;
    this->OSName = OSName;
    this->version = version;
    this->clientType = clientType;
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

quint32 ClientInfo::getClientType() {
    return this->clientType;
}

QString ClientInfo::getAddressStr() {
    return this->address.toString();
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
