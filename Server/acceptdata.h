#ifndef ACCEPTDATA_H
#define ACCEPTDATA_H

#include <QObject>
#include <QUdpSocket>
#include <QHostAddress>

class AcceptData : public QObject
{
    Q_OBJECT
public:
    explicit AcceptData(int portIn, int portOut, QObject *parent = 0);
    ~AcceptData();
    int getPortIn();
    int getPortOut();

private:

    int portIn;
    int portOut;
    QUdpSocket *socket;
    QHostAddress groupAddress;
    QByteArray data;

signals:

public slots:
    void readData();

};

#endif // ACCEPTDATA_H
