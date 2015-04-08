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
#include <QPointer>
#include <QSharedPointer>

#include "g711.h"
#include "recordaudio.h"
#include "recordwav.h"
#include "worker.h"
#include "soundchunk.h"
#include "datagram.h"
#include "channelinfo.h"

class Listener : public Worker
{
    Q_OBJECT
public:
    Listener(Settings *settings);
    ~Listener();

    bool isRecRunning();

private:
    QUdpSocket *socket;
    QHostAddress groupAddress;

    QAudioFormat format;
    QAudioDeviceInfo m_Outputdevice;
    QAudioOutput *m_audioOutput;
    QIODevice *m_output;
    QByteArray m_buffer;
    QMap<qint64, SoundChunk> *outputBuffer;

    qint64 timestamp;
    int binded_port;

    bool isPlaying;

//    short Snack_Alaw2Lin(unsigned char);

    RecordAudio *record;

    void storeChunk(QByteArray);

signals:
    //this signal is emited whenever the player starts listening
    void changePlayButtonState(bool);
    //is emited when a data package is received(updates the GUI speed and transfer size)
    void dataReceived(int);
    //emited when recording is started or stopped
    void changeRecordButtonState(RecordAudio::STATE);
    //emited when recording is paused or reloaded from pause state
    void changePauseButtonState(RecordAudio::STATE);
    //emited when thread work is over
    void finished();

public slots:
    void receiveDatagramm();
    void playback();
    void stopPlayback();
    void volumeChanged(int);
    void portChanged(int);
    void startRecord();
    void pauseRecord();
    void changePlaybackState(QSharedPointer<ChannelInfo> channel);
    void recordingStateChanged(RecordAudio::STATE);

private slots:
    void stopRunning();

};


#endif // LISTENER_H
