#ifndef LISTENNER_H
#define LISTENNER_H

#include <QObject>
#include <QUdpSocket>
#include <QtNetwork>
#include <QAudioFormat>
#include <QAudioDeviceInfo>
#include <QAudioOutput>
#include "gui.h"
#include <QIODevice>
#include <QAudioOutput>
#include <QBuffer>
#include <QTimer>

class Listenner : public QObject
{
    Q_OBJECT
public:
    explicit Listenner(GUI *gui, QObject *parent = 0);

private:
    QUdpSocket *socket;
    QByteArray *buffer;
    GUI *gui;
    QHostAddress groupAddress;

    QAudioFormat format;
    QAudioDeviceInfo m_Outputdevice;
    QAudioOutput *m_audioOutput;
    QIODevice *m_output;
    QByteArray m_buffer;


signals:

public slots:
    void receiveDatagramm();
    void playback();
    void stopPlayback();
    void volumeChanged();
    void portChanged(int);

};

#endif // LISTENNER_H
