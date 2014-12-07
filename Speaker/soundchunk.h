#ifndef SOUNDCHUNK_H
#define SOUNDCHUNK_H

#include <QObject>
#include <QDebug>
#include <cstdint>
#include <QIODevice>
#include <QDataStream>

class SoundChunk
{
public:
    SoundChunk(uint32_t, uint32_t, QString, QByteArray*);
    SoundChunk(QByteArray*);

    ~SoundChunk();

    QByteArray* serialize();
    QByteArray getRawSound();

    static const int CODEC_LENGTH;

private:
    QByteArray soundPacket;
    uint32_t frekv;
    uint32_t channels;
    char codec[20];
    uint32_t chunkSize;

    int headersize;

    QByteArray *serializedPacket;


};

#endif // SOUNDCHUNK_H
