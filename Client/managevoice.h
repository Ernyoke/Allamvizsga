#ifndef MANAGEVOICE_H
#define MANAGEVOICE_H

#include <QObject>
#include <QAudioFormat>
#include <QAudioDeviceInfo>
#include <QAudioInput>
#include "gui.h"
#include "settings.h"
#include <QUdpSocket>
#include <QDateTime>

class ManageVoice : public QObject
{
    Q_OBJECT
public:
    explicit ManageVoice(QUdpSocket *socket, GUI *gui, QObject *parent = 0);

private:
    QAudioInput *audioInput;
    QIODevice *intermediateDevice;
    GUI *gui;
    QUdpSocket *socket;

    Settings *settings;
    int broadcasting_port;

signals:

public slots:
    void startRecording();
    void stopRecording();
    void transferData();

};

#endif // MANAGEVOICE_H
