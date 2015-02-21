#ifndef LISTENERCLIENTINFO_H
#define LISTENERCLIENTINFO_H

#include "clientinfo.h"

class ListenerClientInfo : public ClientInfo
{
public:
    ListenerClientInfo(QHostAddress address, QString OSName, double version);
    ListenerClientInfo(ListenerClientInfo&);
    ListenerClientInfo(ListenerClientInfo*);
    ~ListenerClientInfo();

    CLIENT_TYPE getClientType() const;
    QString getClientTypeStr() const;
};

#endif // LISTENERCLIENTINFO_H
