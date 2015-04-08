#ifndef MANAGEVOICE_H
#define MANAGEVOICE_H

#include <QObject>
#include <QAudioFormat>
#include <QAudioDeviceInfo>
#include <QAudioInput>
#include <QUdpSocket>
#include <QDateTime>

#include "settings.h"
#include "datagram.h"
#include "soundchunk.h"

class ManageVoice : public QObject
{
    Q_OBJECT
public:
    ManageVoice(Settings *settings, QObject *parent = 0);
    ~ManageVoice();

    void showGUI();
    bool isRunning();

private:
    QAudioInput *audioInput;
    QAudioFormat format;
    QIODevice *intermediateDevice;
    QUdpSocket *socket;

    Settings *settings;
    int broadcasting_port;
    int buffLen;

    bool isRecording;

    qint64 timestamp;

    bool checkIP(QString);

signals:
    void recordingState(bool);
    //is emited when a datachunk is sent
    void dataSent(int);
    //is emited when the thread stops
    //errormessage
    void errorMessage(QString);
    //emiter when worker finished his job
    void finished();

public slots:
    void startRecording();
    void stopRecording();
    void transferData();
    void changeRecordState(QAudioFormat speakerFormat);
    void stopWorker();
};

#endif // MANAGEVOICE_H
