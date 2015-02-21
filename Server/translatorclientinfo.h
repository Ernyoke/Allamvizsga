#ifndef TRANSLATORCLIENTINFO_H
#define TRANSLATORCLIENTINFO_H

#include "clientinfo.h"

class TranslatorClientInfo : public ClientInfo
{
public:
    TranslatorClientInfo(QHostAddress address, QString OSName, double version);
    ~TranslatorClientInfo();

    CLIENT_TYPE getClientType() const;
    QString getClientTypeStr() const;
};

#endif // TRANSLATORCLIENTINFO_H
