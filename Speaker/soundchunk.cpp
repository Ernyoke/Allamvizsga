#include "soundchunk.h"

const int SoundChunk::CODEC_LENGTH = 20;

SoundChunk::SoundChunk(quint32 frekv, quint32 channels, QString codec, QByteArray *soundPacket) {
    this->frekv = frekv;
    this->channels = channels;
    strcpy(this->codec, codec.toUtf8().data());
    qDebug() << this->codec;
    this->soundPacket.append(*soundPacket);
    this->chunkSize = soundPacket->size();
    this->headersize = sizeof(frekv) + sizeof(channels) + CODEC_LENGTH + sizeof(chunkSize);
}

SoundChunk::SoundChunk(QByteArray *data) {
    QDataStream stream(data, QIODevice::ReadOnly);
    stream >> frekv;
    stream >> channels;
    int x = stream.readRawData(codec, CODEC_LENGTH);
    stream >> chunkSize;
    this->headersize = sizeof(frekv) + sizeof(channels) + CODEC_LENGTH + sizeof(chunkSize);
    QByteArray temp(data->mid(headersize, chunkSize + headersize));
    soundPacket.append(temp);
}

SoundChunk::~SoundChunk(){
    //
}

QByteArray* SoundChunk::serialize() {
    this->serializedPacket = new QByteArray;
    QDataStream stream(serializedPacket, QIODevice::WriteOnly);
    stream << frekv;
    stream << channels;
    stream.writeRawData(codec, 20);
    quint32 size = soundPacket.size();
    stream << size;
    serializedPacket->append(soundPacket);
    return serializedPacket;
}

QByteArray SoundChunk::getRawSound() {
    return soundPacket;
}
