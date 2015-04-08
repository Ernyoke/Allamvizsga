#ifndef RECORDAUDIO_H
#define RECORDAUDIO_H

#include <QObject>
#include <QFile>
#include <QAudioFormat>
#include <QDebug>
#include <QDateTime>

class RecordAudio : public QObject
{
    Q_OBJECT
public:

    enum STATE {RECORDING, STOPPED, PAUSED};

    explicit RecordAudio(QString, QAudioFormat, QObject *parent = 0);
    ~RecordAudio();
    virtual bool start() = 0;
    virtual bool stop() = 0;
    virtual bool pause() = 0;
    virtual bool write(QByteArray) = 0;
    STATE getState();

protected:
    QFile *outputfile;
    QAudioFormat format;
    STATE currentState;
    QString path;

    bool finalized;

    virtual void finalize() = 0;
    virtual void file() = 0;
    virtual QString extension() = 0;

signals:
    void recordingState(RecordAudio::STATE);


public slots:

};

#endif // RECORDAUDIO_H
