#ifndef SOUNDCHUNK_H
#define SOUNDCHUNK_H

#include <QObject>
#include <QDebug>
#include <QIODevice>
#include <QDataStream>

class SoundChunk
{
public:
    SoundChunk(quint32, quint32, QString, QByteArray*);
    SoundChunk(QByteArray*);

    ~SoundChunk();

    QByteArray* serialize();
    QByteArray getRawSound();

    quint32 getSize();

private:
    QByteArray soundPacket;
    quint32 frekv;
    quint32 channels;
    char codec[20];
    quint32 chunkSize;

    QByteArray *serializedPacket;

    static const int CODEC_LENGTH;

    quint32 headerSize();

};


#endif // SOUNDCHUNK_H
