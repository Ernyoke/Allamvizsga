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
    explicit Speaker(QUdpSocket *socket, GUI *gui, QObject *parent = 0);

private:
    QAudioInput *audioInput;
    QIODevice *intermediateDevice;
    GUI *gui;
    QUdpSocket *socket;
    QDateTime *time;

    Settings *settings;

    int broadcasting_port;

    unsigned char Snack_Lin2Alaw(short);
    short search(short, short*, short);

protected:
    void run();

signals:

public slots:
    void startRecording();
    void stopRecording();
    void transferData();

};

#endif // SPEAKER_H
