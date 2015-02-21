#ifndef SPEAKERCLIENTINFO_H
#define SPEAKERCLIENTINFO_H

#include "clientinfo.h"

class SpeakerClientInfo : public ClientInfo
{
public:
    SpeakerClientInfo(QHostAddress address, QString OSName, double version);
    SpeakerClientInfo(SpeakerClientInfo &);
    SpeakerClientInfo(SpeakerClientInfo *);
    ~SpeakerClientInfo();

    CLIENT_TYPE getClientType() const;
    QString getClientTypeStr() const;
};

#endif // SPEAKERCLIENTINFO_H
