#ifndef ABSTRACTSPEAKER_H
#define ABSTRACTSPEAKER_H

#include <QObject>

#include "worker.h"

class AbstractSpeaker : public Worker
{
    Q_OBJECT
public:
    AbstractSpeaker();
    ~AbstractSpeaker();

protected:
    bool isRecording;
    qint64 timestamp;
    qint32 clientId;

    int broadcasting_port;
    int buffLen;

signals:
    //is recording or not
    void recordingState(bool);
    //is emited when a datachunk is sent
    void dataSent(int);

protected slots:
    virtual void transferData() = 0;

public slots:
    virtual void start(QAudioFormat speakerFormat, QAudioDeviceInfo device,
               QHostAddress serverAddress, qint32 broadcasting_port, qint32 clientId) = 0;
    virtual void stop() = 0;


public slots:
};

#endif // ABSTRACTSPEAKER_H
