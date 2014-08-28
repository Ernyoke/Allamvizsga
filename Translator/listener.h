#ifndef LISTENER_H
#define LISTENER_H

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
#include <QThread>
#include <QMap>

#include "g711.h"
#include "settings.h"
#include "recordaudio.h"
#include "recordwav.h"

class Listener : public QThread
{
    Q_OBJECT
public:
    explicit Listener(GUI *gui, QObject *parent = 0);
    ~Listener();

private:
    QUdpSocket *socket;
    GUI *gui;
    QHostAddress groupAddress;

    QAudioFormat format;
    QAudioDeviceInfo m_Outputdevice;
    QAudioOutput *m_audioOutput;
    QIODevice *m_output;
    QByteArray m_buffer;
    QMap<qint64, QByteArray> *outputBuffer;

    qint64 timestamp;
    int binded_port;

    Settings *settings;

    bool isPlaying;

    short Snack_Alaw2Lin(unsigned char);

    RecordAudio *record;

    void storeChunk(QByteArray);

protected:
    void run();

signals:

public slots:
    void receiveDatagramm();
    void playback();
    void stopPlayback();
    void volumeChanged();
    void portChanged(int);
    void startRecord();
    void pauseRecord();
    void changePlaybackState();

};


#endif // LISTENER_H
