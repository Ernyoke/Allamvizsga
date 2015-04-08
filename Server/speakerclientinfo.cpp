#include "speakerclientinfo.h"

SpeakerClientInfo::SpeakerClientInfo(QHostAddress address, QString OSName, double version) :
    ClientInfo(address, OSName, version)
{

}

SpeakerClientInfo::SpeakerClientInfo(SpeakerClientInfo &speaker) :
    ClientInfo(speaker.getAddress(), speaker.getOSName(), 0)
{

}

SpeakerClientInfo::SpeakerClientInfo(SpeakerClientInfo *speaker) :
    ClientInfo(speaker->getAddress(), speaker->getOSName(), 0)
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

