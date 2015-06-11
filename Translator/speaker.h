#ifndef SPEAKER_H
#define SPEAKER_H

#include <QObject>
#include <QAudioFormat>
#include <QAudioDeviceInfo>
#include <QAudioInput>
#include <QUdpSocket>
#include <QDateTime>

#include "datagram.h"
#include "soundchunk.h"
#include "abstractspeaker.h"

class Speaker : public AbstractSpeaker
{
    Q_OBJECT
public:
    Speaker();
    ~Speaker();

protected:
    QAudioInput *audioInput;
    QIODevice *intermediateDevice;

signals:

protected slots:
    virtual void transferData();

public slots:
    virtual void start(QAudioFormat speakerFormat, QAudioDeviceInfo device,
               QHostAddress serverAddress, qint32 broadcasting_port, qint32 clientId);
    virtual void stop();
};

#endif // SPEAKER_H
