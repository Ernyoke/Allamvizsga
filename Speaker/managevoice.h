#ifndef MANAGEVOICE_H
#define MANAGEVOICE_H

#include <QObject>
#include <QAudioFormat>
#include <QAudioDeviceInfo>
#include <QAudioInput>
#include "settings.h"
#include "datagram.h"
#include "soundchunk.h"
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
    QHostAddress *IPAddress;
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

public slots:
    void startRecording();
    void stopRecording();
    void transferData();
    void changeRecordState(QString, QString);
};

#endif // MANAGEVOICE_H
