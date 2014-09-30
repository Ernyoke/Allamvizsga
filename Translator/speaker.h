#ifndef SPEAKER_H
#define SPEAKER_H

#include <QObject>
#include <QAudioFormat>
#include <QAudioDeviceInfo>
#include <QAudioInput>
#include <QUdpSocket>
#include <QThread>
#include <QDateTime>

#include "g711.h"
#include "worker.h"

class Speaker : public Worker
{
    Q_OBJECT
public:
    Speaker(Settings*);
    ~Speaker();

private:

    QAudioInput *audioInput;
    QAudioFormat format;
    QIODevice *intermediateDevice;
    QUdpSocket *socket;

    int broadcasting_port;
    qint64 timestamp;

    bool isRecording;
    int buffLen;

    unsigned char Snack_Lin2Alaw(short);
    short search(short, short*, short);


signals:
    void recordingState(bool);
    void dataSent(int);
    void finished();

public slots:
    void startRecording();
    void stopRecording();
    void changeRecordState(int);
    void transferData();
    void stopRunning();

};

#endif // SPEAKER_H
