#ifndef TESTSPEAKER_H
#define TESTSPEAKER_H

#include <QObject>
#include <QFile>
#include <QDataStream>
#include <QTimer>
#include <QDebug>
#include "abstractspeaker.h"
#include "datagram.h"

class TestSpeaker : public AbstractSpeaker
{
    Q_OBJECT
public:
    TestSpeaker();
    ~TestSpeaker();

private:
    QFile *file;
    QTimer *timer;

    int timerId;

    int calcBufferSize(QAudioFormat &, int interval);

protected:
    void timerEvent(QTimerEvent * event);

signals:

private slots:
    void transferData();

public slots:
    void start(QAudioFormat speakerFormat, QAudioDeviceInfo device,
               QHostAddress serverAddress, qint32 broadcasting_port, qint32 clientId);
    void stop();

};

#endif // TESTSPEAKER_H
