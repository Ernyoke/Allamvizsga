#ifndef SOUNDCHUNK_H
#define SOUNDCHUNK_H

#include <QObject>
#include <QDebug>
#include <QIODevice>
#include <QDataStream>

class SoundChunk
{
public:
    SoundChunk(qint32 sampleRate, qint32 sampleSize, qint32, QString, QByteArray*);
    SoundChunk(QByteArray*);

    ~SoundChunk();

    QByteArray serialize();
    QByteArray getRawSound();

    static const int CODEC_LENGTH;

    qint32 getSize() const;
    qint32 getSampleRate() const;
    qint32 getSampleSize() const;
    QString getCodec() const;

private:
    QByteArray soundPacket;
    qint32 sampleRate;
    qint32 sampleSize;
    qint32 channels;
    QString codec;


};


#endif // SOUNDCHUNK_H
