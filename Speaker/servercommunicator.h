#ifndef SERVERCOMMUNICATOR_H
#define SERVERCOMMUNICATOR_H

#include <QObject>
#include <QUdpSocket>

#include "settings.h"
#include "datagram.h"

class ServerCommunicator : public QObject
{
    Q_OBJECT
public:
    explicit ServerCommunicator(Settings *settings, QObject *parent = 0);
    ~ServerCommunicator();

private:
    QUdpSocket *socket;
    Settings *settings;

    void processDatagram(Datagram);

private slots:
    void sendLoginRequest(Datagram);
    void sendDatagram(Datagram);
    void readDatagram();

signals:
    void loginAckReceived(Datagram);
    void newChannelAckReceived(Datagram);

public slots:
};

#endif // SERVERCOMMUNICATOR_H
