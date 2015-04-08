#ifndef SERVERCOMMUNICATOR_H
#define SERVERCOMMUNICATOR_H

#include <QObject>
#include <QUdpSocket>
#include <QTimer>
#include <QMap>

#include "settings.h"
#include "datagram.h"
#include "channelinfo.h"

class ServerCommunicator : public QObject
{
    Q_OBJECT
public:
    explicit ServerCommunicator(Settings *settings, QObject *parent = 0);
    ~ServerCommunicator();

private:
    QUdpSocket *socket;
    Settings *settings;
    QTimer *listReqTimer;
    QTimer *refreshTimer;
    QTimer *loginTimer;
    int refreshCounter;

    //true if client is authentificated, otherwhise false
    bool authentificationStatus;

    qint64 reqListStart;
    QMap<qint32, QByteArray> listContent;

    void processDatagram(Datagram&);
    void processList(Datagram&);
    void processNewChannel(Datagram&);
    void processSynch(Datagram&);
    void processServerDown(Datagram&);
    void processLogin(Datagram&);
    void processLogout(Datagram&);
    void processRemoveChannel(Datagram&);

private slots:
    void sendLoginRequest();
    void logout();
    void sendDatagram(Datagram *);
    void sendDatagram(Datagram);
    void readDatagram();
    void determineServerStatus();
    void listReqTimedOut();
    void loginTimedOut();

public slots:
    void requestChannelList();

signals:
    void authentificationSucces(qint32);
    void authentificationFailed();
    void authentificationTimedOut();
//    void newChannelAckReceived(Datagram);
    void serverList(QByteArray);
    void channelConnected(ChannelInfo);
    void serverDown();
    void removeChannel(qint32);

public slots:
};

#endif // SERVERCOMMUNICATOR_H
