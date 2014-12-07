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
#include "soundchunk.h"
#include "datagram.h"

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
    QHostAddress *IPAddress;

    int broadcasting_port;
    qint64 timestamp;

    bool isRecording;
    int buffLen;

    unsigned char Snack_Lin2Alaw(short);
    short search(short, short*, short);

    bool checkIP(QString);


signals:
    //is emited when the recordig state(recording or stoped) is changed
    void recordingState(bool);
    //is emited when a datachunk is sent
    void dataSent(int);
    //is emited when the thread stops
    void finished();
    //
    void errorMessage(QString);

public slots:
    void startRecording();
    void stopRecording();
    void changeRecordState(QString, QString);
    void transferData();
    void stopRunning();

};

#endif // SPEAKER_H
