#ifndef SERVERCOMMUNICATOR_H
#define SERVERCOMMUNICATOR_H

#include <QObject>
#include <QUdpSocket>
#include <QTimer>

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

    qint64 reqListStart;
    QMap<qint32, QByteArray> listContent;

    void processDatagram(Datagram&);
    void processList(Datagram&);
    void processNewChannel(Datagram&);
    void processSynch(Datagram&);

private slots:
    void sendLoginRequest(Datagram);
    void sendDatagram(Datagram);
    void readDatagram();

public slots:
    void requestChannelList();

signals:
    void loginAckReceived(Datagram);
    void newChannelAckReceived(Datagram);
    void serverList(QByteArray);
    void channelConnected(ChannelInfo);

public slots:
    void listReqTimedOut();
};

#endif // SERVERCOMMUNICATOR_H
