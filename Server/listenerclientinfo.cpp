#include "listenerclientinfo.h"

ListenerClientInfo::ListenerClientInfo(QHostAddress address, qint32 port, QString OSName, double version) :
    ClientInfo(address, port, OSName, version)
{
    //
}

ListenerClientInfo::ListenerClientInfo(ListenerClientInfo &listener) :
    ClientInfo(listener.getAddress(), listener.getClientPort(), listener.getOSName(), 0)
{
    //
}

ListenerClientInfo::ListenerClientInfo(ListenerClientInfo *listener) :
    ClientInfo(listener->getAddress(), listener->getClientPort(), listener->getOSName(), 0)
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

