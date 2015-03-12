#ifndef TRANSLATORCLIENTINFO_H
#define TRANSLATORCLIENTINFO_H

#include "listenerclientinfo.h"
#include "speakerclientinfo.h"

class TranslatorClientInfo : public ListenerClientInfo, public SpeakerClientInfo
{
public:
    TranslatorClientInfo(QHostAddress address, QString OSName, double version);
    ~TranslatorClientInfo();

    CLIENT_TYPE getClientType() const;
    QString getClientTypeStr() const;
};

#endif // TRANSLATORCLIENTINFO_H
