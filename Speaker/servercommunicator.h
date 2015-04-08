#ifndef SERVERCOMMUNICATOR_H
#define SERVERCOMMUNICATOR_H

#include <QObject>
#include <QUdpSocket>
#include <QTimer>

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
    QTimer *refreshTimer;
    QTimer *loginTimer;
    int refreshCounter;

    //true if client is authentificated, otherwhise false
    bool authentificationStatus;

    void processDatagram(Datagram&);
    void processSynch(Datagram&);
    void processServerDown(Datagram&);
    void processLogin(Datagram&);
    void processLogout(Datagram&);

private slots:
    void sendLoginRequest();
    void logout();
    void sendDatagram(Datagram *);
    void sendDatagram(Datagram);
    void readDatagram();
    void determineServerStatus();
    void loginTimedOut();

signals:
    void authentificationSucces(qint32);
    void authentificationFailed();
    void authentificationTimedOut();
    void newChannelAckReceived(Datagram);
    void serverDown();

public slots:
};

#endif // SERVERCOMMUNICATOR_H
