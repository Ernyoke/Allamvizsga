#ifndef ACCEPTDATA_H
#define ACCEPTDATA_H

#include <QObject>
#include <QUdpSocket>
#include <QHostAddress>
#include <QThread>

class AcceptData : public QThread
{
    Q_OBJECT
public:
    explicit AcceptData(int portIn, int portOut, QObject *parent = 0);

    int getPortIn();
    int getPortOut();

private:
    void run();

    int portIn;
    int portOut;
    QUdpSocket *socket;
    QHostAddress groupAddress;

signals:

public slots:
    void readData();

};

#endif // ACCEPTDATA_H
