#ifndef WORKER_H
#define WORKER_H

#include <QObject>
#include "settings.h"

class Worker : public QObject
{
    Q_OBJECT
public:
    Worker(Settings*);

protected:
    Settings *settings;

signals:
    void errorMessage(QString);

public slots:
    virtual void stopRunning() = 0;

};

#endif // WORKER_H
