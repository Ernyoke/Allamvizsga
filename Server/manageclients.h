#ifndef MANAGECLIENTS_H
#define MANAGECLIENTS_H

#include <QObject>
#include <QUdpSocket>
#include <QTimer>

#include "clientinfo.h"
#include "speakerclientinfo.h"
#include "listenerclientinfo.h"
#include "translatorclientinfo.h"
#include "datagram.h"
#include "clientmodel.h"
#include "channelinfo.h"
#include "channelmodel.h"
#include "packetlogger.h"

class ManageClients : public PacketLogger
{
    Q_OBJECT

public:
    ManageClients(ClientModel *tableModel, ChannelModel *channelModel, QMutex *mutex);
    ~ManageClients();

private:

    QUdpSocket *socket;

    qint32 clientID;
    qint16 outPort;

    ClientModel *clientModel;
    ChannelModel *channelModel;

    QTimer *synchTimer;

    void processDatagram(Datagram, QHostAddress address, quint16 port);
    bool nextClientId();
    bool isAvNextClient();
    bool nextPort();
    bool isPortAv();
    void resolveLogin(QByteArray *content, QHostAddress address, qint32 port, qint64 timeStamp);
    void newChannel(const Datagram &dgram, QHostAddress &address, qint32 port);
    void closeChannel(const Datagram& dgram);
    void synchResponse(const Datagram& dgram);
    void clientDisconnected(const Datagram& dgram);

private slots:
    void readPendingDatagrams();
    void sendCollectiveMessageToSpeakers(Datagram &dgram);
    void sendCollectiveMessageToListeners(Datagram &dgram);
    void sendCollectiveMessage(Datagram &dgram);
    void synchronizeClients();
    void sendDatagram(QHostAddress &, qint32, Datagram &);

public slots:
    void serverDown();

signals:
    void newChannelAdded(ChannelInfo);
    void channelClosed(qint32);
    void newClientConnected(ClientInfo*);
    void clientConnectionAck(qint32);
    void clientDisconnectedSignal(qint32);

};

#endif // MANAGECLIENTS_H
