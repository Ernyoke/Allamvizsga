#include "soundchunk.h"

SoundChunk::SoundChunk(qint32 sampleRate, qint32 sampleSize, qint32 channels, QString codec, QByteArray *soundPacket) {
    this->sampleRate = sampleRate;
    this->sampleSize = sampleSize;
    this->channels = channels;
    this->codec = codec;
    this->soundPacket.append(*soundPacket);
}

SoundChunk::SoundChunk(QByteArray *data) {
    QDataStream stream(data, QIODevice::ReadOnly);
    stream >> sampleRate;
    stream >> sampleSize;
    stream >> channels;
    stream >> codec;
    stream >> soundPacket;
}

SoundChunk::~SoundChunk(){
    //
}

QByteArray SoundChunk::serialize() {
    QByteArray serializedPacket;
    QDataStream stream(&serializedPacket, QIODevice::WriteOnly);
    stream << sampleRate;
    stream << sampleSize;
    stream << channels;
    stream << codec;
    stream << soundPacket;
    return serializedPacket;
}

QByteArray SoundChunk::getRawSound() {
    return soundPacket;
}


qint32 SoundChunk::getSize() const {
    qint32 size = sizeof(sampleRate) + sizeof(sampleSize) + sizeof(channels) + sizeof(quint32) + 2 * codec.length() +
            sizeof(quint32) + soundPacket.size();
    return size;
}

qint32 SoundChunk::getSampleRate() const {
    return this->sampleRate;
}

qint32 SoundChunk::getSampleSize() const {
    return this->sampleSize;
}

QString SoundChunk::getCodec() const {
    return this->codec;
}
