#include "translatorclientinfo.h"

TranslatorClientInfo::TranslatorClientInfo(QHostAddress address, qint32 port, QString OSName, double version) :
    ClientInfo(address, port, OSName, version), SpeakerClientInfo(address, port, OSName, version), ListenerClientInfo(address, port, OSName, version)
{

}

TranslatorClientInfo::~TranslatorClientInfo()
{

}

ClientInfo::CLIENT_TYPE TranslatorClientInfo::getClientType() const {
    return TRANSLATOR;
}

QString TranslatorClientInfo::getClientTypeStr() const {
    return "Translator";
}

