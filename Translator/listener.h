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
#include "recordaudio.h"
#include "recordwav.h"
#include "worker.h"

class Listener : public Worker
{
    Q_OBJECT
public:
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
    //emitted when recording is started or stopped
    void changeRecordButtonState(RecordAudio::STATE);
    //emitted when recording is paused or reloaded from pause state
    void changePauseButtonState(RecordAudio::STATE);
    //emitted when saving the recorded sound(renaming the file)
    void askFileNameGUI(QString);
    //emit when thread work is over
    void finished();
    //error message
    void showError(QString);

public slots:
    void receiveDatagramm();
    void playback();
    void stopPlayback();
    void volumeChanged(int);
    void portChanged(int);
    void startRecord();
    void pauseRecord();
    void changePlaybackState(int);

private slots:
    void askFileName(QString filename);
    void stopRunning();

};


#endif // LISTENER_H
