#include "translatorclientinfo.h"

TranslatorClientInfo::TranslatorClientInfo(QHostAddress address, QString OSName, double version) :
    ClientInfo(address, OSName, version), SpeakerClientInfo(address, OSName, version), ListenerClientInfo(address, OSName, version)
{

}

TranslatorClientInfo::~TranslatorClientInfo()
{

}

ClientInfo::CLIENT_TYPE TranslatorClientInfo::getClientType() const {
    return CLIENT_TYPE::TRANSLATOR;
}

QString TranslatorClientInfo::getClientTypeStr() const {
    return "Translator";
}

