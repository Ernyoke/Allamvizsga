#include "channelinfo.h"

ChannelInfo::ChannelInfo() {

}

ChannelInfo::ChannelInfo(qint32 userid, QString language, QString codec, qint32 sample_rate, qint32 sample_size, qint32 channels)
{
    this->language = language;
    this->codec = codec;
    this->sample_rate = sample_rate;
    this->sample_size = sample_size;
    this->channels = channels;
    this->ownerId = userid;


}

ChannelInfo::ChannelInfo(QByteArray &content) {
    deserialize(content);
}

//ChannelInfo::ChannelInfo(const ChannelInfo *info) {
//    this->language = info->getLanguage();
//    this->codec = info->getCodec();
//    this->sample_rate = info->getSampleRate();
//    this->sample_size = info->getSampleSize();
//    this->channels = info->getChannels();
//    this->ownerId = info->getOwner();
//    this->port = info->getOutPort();
//}

ChannelInfo::ChannelInfo(const ChannelInfo &info) {
    this->language = info.getLanguage();
    this->codec = info.getCodec();
    this->sample_rate = info.getSampleRate();
    this->sample_size = info.getSampleSize();
    this->channels = info.getChannels();
    this->ownerId = info.getOwner();
    this->port = info.getOutPort();
}

ChannelInfo::~ChannelInfo()
{

}

QByteArray ChannelInfo::serialize() {
    QByteArray content;
    QDataStream out(&content, QIODevice::WriteOnly);
    out << ownerId;
    out << port;
    out << sample_rate;
    out << sample_size;
    out << channels;
    out << codec;
    out << language;
    return content;
}

void ChannelInfo::deserialize(QByteArray &content) {
     QDataStream in(&content, QIODevice::ReadOnly);
     in >> ownerId;
     in >> port;
     in >> sample_rate;
     in >> sample_size;
     in >> channels;
     in >> codec;
     in >> language;
}

qint32 ChannelInfo::getSampleRate() const {
    return this->sample_rate;
}

qint32 ChannelInfo::getSampleSize() const {
    return this->sample_size;
}

qint32 ChannelInfo::getChannels() const {
    return this->channels;
}

QString ChannelInfo::getCodec() const {
    return this->codec;
}

QString ChannelInfo::getLanguage() const {
    return this->language;
}

qint32 ChannelInfo::getOwner() const {
    return this->ownerId;
}

void ChannelInfo::setOutPort(qint16 port) {
    this->port = port;
}

qint16 ChannelInfo::getOutPort() const {
    return this->port;
}
