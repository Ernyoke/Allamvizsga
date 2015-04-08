#include "listenerclientinfo.h"

ListenerClientInfo::ListenerClientInfo(QHostAddress address, QString OSName, double version) :
    ClientInfo(address, OSName, version)
{
    //
}

ListenerClientInfo::ListenerClientInfo(ListenerClientInfo &listener) :
    ClientInfo(listener.getAddress(), listener.getOSName(), 0)
{
    //
}

ListenerClientInfo::ListenerClientInfo(ListenerClientInfo *listener) :
    ClientInfo(listener->getAddress(), listener->getOSName(), 0)
{
    //
}

ListenerClientInfo::~ListenerClientInfo()
{

}

ClientInfo::CLIENT_TYPE ListenerClientInfo::getClientType() const {
    return LISTENER;
}

QString ListenerClientInfo::getClientTypeStr() const {
    return "Listener";
}

