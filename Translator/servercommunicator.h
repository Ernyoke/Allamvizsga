#ifndef SERVERCOMMUNICATOR_H
#define SERVERCOMMUNICATOR_H

#include <QObject>
#include <QUdpSocket>
#include <QTimer>
#include <QMap>

#include "settings.h"
#include "datagram.h"
#include "channelinfo.h"
#include "packetlogger.h"

class ServerCommunicator : public QObject, public PacketLogger
{
    Q_OBJECT
public:
    explicit ServerCommunicator(QObject *parent = 0);
    ~ServerCommunicator();

    QHostAddress getServerAddress() const;
    qint32 getClientId() const;
    qint32 getClientPort() const;
    qint32 getServerPort() const;
    qint32 getClientPortForSound() const;

private:
    QUdpSocket *socket;
    QTimer *listReqTimer;
    QTimer *refreshTimer;
    QTimer *loginTimer;
    int refreshCounter;

    //true if client is authentificated, otherwhise false
    bool authentificationStatus;

    qint64 reqListStart;
    QMap<qint32, QByteArray> listContent;

    qint32 myclientId;
    QHostAddress serverAddress;
    qint32 serverPort;
    qint32 clientPort;
    qint32 clientPortForSound;

    void processDatagram(Datagram&);
    void processList(Datagram&);
    void processNewChannel(Datagram&);
    void processSynch(Datagram&);
    void processServerDown(Datagram&);
    void processLogin(Datagram&);
    void processLogout(Datagram&);
    void processRemoveChannel(Datagram&);

private slots:
    void sendLoginRequest(QString address);
    void logout();
    void sendDatagram(Datagram *);
    void sendDatagram(Datagram);
    void readDatagram();
    void determineServerStatus();
    void listReqTimedOut();
    void loginTimedOut();

public slots:
    void requestChannelList();
    void startPacketLog(QMutex *mutex, QFile *file);
    void stopPacketLog();


signals:
    void authentificationSucces(qint32);
    void authentificationFailed();
    void authentificationTimedOut();
    void newChannelAckReceived(Datagram);
    void serverList(QByteArray);
    void channelConnected(ChannelInfo);
    void serverDown();
    void removeChannel(qint32);

public slots:
};

#endif // SERVERCOMMUNICATOR_H
