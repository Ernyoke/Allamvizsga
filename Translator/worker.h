#ifndef WORKER_H
#define WORKER_H

#include <QObject>
#include <QUdpSocket>
#include <QAudioDeviceInfo>
#include <QAudioFormat>
#include <QPointer>

#include "soundchunk.h"

class Worker : public QObject
{
    Q_OBJECT
public:
    Worker();

    virtual ~Worker();

protected:

    QHostAddress serverAddress;
    QPointer<QUdpSocket> socket;
    QAudioFormat format;

signals:
    void errorMessage(QString);
    void finished();

public slots:

};

#endif // WORKER_H
