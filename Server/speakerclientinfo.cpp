#include "speakerclientinfo.h"

SpeakerClientInfo::SpeakerClientInfo(QHostAddress address, qint32 clientPort, QString OSName, double version) :
    ClientInfo(address, clientPort, OSName, version)
{

}

SpeakerClientInfo::SpeakerClientInfo(SpeakerClientInfo &speaker) :
    ClientInfo(speaker.getAddress(), speaker.getClientPort(), speaker.getOSName(), 0)
{

}

SpeakerClientInfo::SpeakerClientInfo(SpeakerClientInfo *speaker) :
    ClientInfo(speaker->getAddress(), speaker->getClientPort(), speaker->getOSName(), 0)
{

}

SpeakerClientInfo::~SpeakerClientInfo()
{

}

ClientInfo::CLIENT_TYPE SpeakerClientInfo::getClientType() const {
    return SPEAKER;
}

QString SpeakerClientInfo::getClientTypeStr() const {
    return "Speaker";
}

