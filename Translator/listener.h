#ifndef LISTENER_H
#define LISTENER_H

#include <QObject>
#include <QUdpSocket>
#include <QtNetwork>
#include <QAudioFormat>
#include <QAudioDeviceInfo>
#include <QAudioOutput>
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

class Listener : public QObject
{
    Q_OBJECT
public:
    explicit Listener(QObject *parent = 0);
    Listener(Settings *settings);
    ~Listener();

private:
    QUdpSocket *socket;
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

//    short Snack_Alaw2Lin(unsigned char);

    RecordAudio *record;

    void storeChunk(QByteArray);

signals:
    //this signal is emited whenever the player starts listening
    void changePlayButtonState(bool);
    //is emited when a data package is received(updates the GUI speed and transfer size)
    void dataReceived(int);
    void finished();

public slots:
    void receiveDatagramm();
    void playback();
    void stopPlayback();
    void volumeChanged(int);
    void portChanged(int);
    void startRecord();
    void pauseRecord();
    void changePlaybackState(int);

    void stopRunning();

private slots:


};


#endif // LISTENER_H
