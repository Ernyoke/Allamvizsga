#ifndef MANAGEVOICE_H
#define MANAGEVOICE_H

#include <QObject>
#include <QAudioFormat>
#include <QAudioDeviceInfo>
#include <QAudioInput>
#include "settings.h"
#include <QUdpSocket>
#include <QDateTime>

class ManageVoice : public QObject
{
    Q_OBJECT
public:
    ManageVoice(QObject *parent = 0, Settings *settings = 0);
    ~ManageVoice();

    void showGUI();

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

signals:
    void recordingState(bool);
    //is emited when a datachunk is sent
    void dataSent(int);
    //is emited when the thread stops

public slots:
    void startRecording();
    void stopRecording();
    void transferData();
    void changeRecordState(int);
};

#endif // MANAGEVOICE_H
