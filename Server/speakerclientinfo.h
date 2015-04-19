#ifndef SPEAKERCLIENTINFO_H
#define SPEAKERCLIENTINFO_H

#include "clientinfo.h"

class SpeakerClientInfo : public virtual ClientInfo
{
public:
    SpeakerClientInfo(QHostAddress address, qint32 clientPort, QString OSName, double version);
    SpeakerClientInfo(SpeakerClientInfo &);
    SpeakerClientInfo(SpeakerClientInfo *);
    virtual ~SpeakerClientInfo();

    virtual CLIENT_TYPE getClientType() const;
    virtual QString getClientTypeStr() const;
};

#endif // SPEAKERCLIENTINFO_H
