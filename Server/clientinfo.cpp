#include "clientinfo.h"

ClientInfo::ClientInfo(QHostAddress address, quint16 port, uint32_t clientType, QString OSName, double version)
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

