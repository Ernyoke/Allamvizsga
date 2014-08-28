#ifndef RECORDAUDIO_H
#define RECORDAUDIO_H

#include <QObject>
#include <QFile>
#include <QAudioFormat>
#include <QDebug>

//dealing with circular depnedencie
class GUI;

class RecordAudio : public QObject
{
    Q_OBJECT
public:

    enum STATE {RECORDING, STOPPED, PAUSED};

    explicit RecordAudio(QString, QAudioFormat, GUI*, QObject *parent = 0);
    ~RecordAudio();
    virtual bool start() = 0;
    virtual bool stop() = 0;
    virtual bool pause() = 0;
    virtual bool write(QByteArray) = 0;
    STATE getState();

protected:
    QFile *outputfile;
    QAudioFormat format;
    GUI* gui;
    STATE currentState;
    QString path;

signals:

public slots:

};

#endif // RECORDAUDIO_H
