#ifndef LISTENERCLIENTINFO_H
#define LISTENERCLIENTINFO_H

#include "clientinfo.h"

class ListenerClientInfo : public virtual ClientInfo
{
public:
    ListenerClientInfo(QHostAddress address, QString OSName, double version);
    ListenerClientInfo(ListenerClientInfo&);
    ListenerClientInfo(ListenerClientInfo*);
    virtual ~ListenerClientInfo();

    virtual CLIENT_TYPE getClientType() const;
    virtual QString getClientTypeStr() const;
};

#endif // LISTENERCLIENTINFO_H
