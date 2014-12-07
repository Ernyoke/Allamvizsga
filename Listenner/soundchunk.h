#ifndef SOUNDCHUNK_H
#define SOUNDCHUNK_H

#include <QObject>
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

private:
    QByteArray soundPacket;
    uint32_t frekv;
    uint32_t channels;
    char codec[20];
    uint32_t chunkSize;

    int headersize;

    QByteArray *serializedPacket;

    static const int CODEC_LENGTH;

};


#endif // SOUNDCHUNK_H
