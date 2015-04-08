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

#include "recordaudio.h"
#include "recordwav.h"
//#include "g711.h"
#include "settings.h"
#include "datagram.h"
#include "channelinfo.h"

class Listener : public QObject
{
    Q_OBJECT
public:
    explicit Listener(Settings *settings = 0);
    ~Listener();

    bool isRecRunning();

private:
    QPointer<QUdpSocket> socket;
    QHostAddress groupAddress;

    QAudioFormat format;
    QAudioDeviceInfo m_Outputdevice;
    QPointer<QAudioOutput> m_audioOutput;
    QPointer<QIODevice> m_output;
    QByteArray m_buffer;
    QMap<qint64, SoundChunk> *outputBuffer;

    qint64 timestamp;
    int binded_port;

    Settings *settings;

    bool isPlaying;

    short Snack_Alaw2Lin(unsigned char);

    QPointer <RecordAudio > record;

    void storeChunk(QByteArray);


signals:
    //this signal is emited whenever the player starts listening
    void changePlayButtonState(bool);
    //is emited when a data package is received(updates the GUI speed and transfer size)
    void dataReceived(int);
    //emitted when recording is started or stopped
    void changeRecordButtonState(RecordAudio::STATE);
    //emitted when recording is paused or reloaded from pause state
    void changePauseButtonState(RecordAudio::STATE);
    //emit when thread work is over
    void finished();
    //error message
    void errorMessage(QString);

private slots:
    void receiveDatagramm();
    void playback();
    void stopPlayback();
    void volumeChanged(qreal);
    void channelChanged(QSharedPointer<ChannelInfo> channel);
    void startRecord();
    void pauseRecord();
    void changePlaybackState(QSharedPointer<ChannelInfo> channel);
    void recordingStateChanged(RecordAudio::STATE);
    void stopWorker();

};

#endif // LISTENER_H
