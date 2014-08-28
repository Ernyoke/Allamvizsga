#ifndef SPEAKER_H
#define SPEAKER_H

#include <QObject>
#include <QAudioFormat>
#include <QAudioDeviceInfo>
#include <QAudioInput>
#include "gui.h"
#include <QUdpSocket>
#include <QThread>
#include <QDateTime>

#include "g711.h"
#include "settings.h"

class Speaker : public QThread
{
    Q_OBJECT
public:
    explicit Speaker(GUI *gui, QObject *parent = 0);
    ~Speaker();

private:
    QAudioInput *audioInput;
    QAudioFormat format;
    QIODevice *intermediateDevice;
    GUI *gui;
    QUdpSocket *socket;

    Settings *settings;

    int broadcasting_port;
    qint64 timestamp;

    bool isRecording;
    int buffLen;

    unsigned char Snack_Lin2Alaw(short);
    short search(short, short*, short);

protected:
    void run();

signals:

public slots:
    void startRecording();
    void stopRecording();
    void changeRecordState();
    void transferData();

};

#endif // SPEAKER_H
