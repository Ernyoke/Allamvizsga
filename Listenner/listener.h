#ifndef LISTENER_H
#define LISTENER_H

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
#include "datagram.h"
#include "channelinfo.h"
#include "worker.h"
#include "noaudiodeviceexception.h"
#include "settings.h"

class Listener : public Worker
{
    Q_OBJECT
public:
    Listener();
    ~Listener();

    bool isRecRunning();

private:
    QPointer<QUdpSocket> socket;
    QHostAddress serverAddress;

    QAudioFormat format;
    QAudioDeviceInfo m_Outputdevice;
    QPointer<QAudioOutput> m_audioOutput;
    QPointer<QIODevice> m_output;
    QByteArray m_buffer;
    QMap<qint64, SoundChunk> *outputBuffer;

    qint64 timestamp;
    int binded_port;

    bool isPlaying;

//    short Snack_Alaw2Lin(unsigned char);

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

public slots:
    void start(QSharedPointer<ChannelInfo> channel, QAudioDeviceInfo device, QHostAddress serverAddress, int volume);
    void stop();
    void volumeChanged(qreal);
    void startRecord(Settings::CODEC codec, QString path);
    void pauseRecord();

private slots:
    void receiveDatagramm();
    void recordingStateChanged(RecordAudio::STATE);

};

#endif // LISTENER_H
