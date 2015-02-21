#ifndef MANAGECLIENTS_H
#define MANAGECLIENTS_H

#include <QObject>
#include <QUdpSocket>

#include "clientinfo.h"
#include "speakerclientinfo.h"
#include "listenerclientinfo.h"
#include "translatorclientinfo.h"
#include "datagram.h"
#include "clientmodel.h"
#include "channelinfo.h"
#include "channelmodel.h"

class ManageClients : public QObject
{
    Q_OBJECT

public:
    explicit ManageClients(ClientModel *tableModel, ChannelModel *channelModel, QWidget *parent = 0);
    ~ManageClients();

private:

    QUdpSocket *socket;

    qint32 clientID;
    qint16 outPort;

    ClientModel *clientModel;
    ChannelModel *channelModel;

    void processDatagram(Datagram, QHostAddress address, quint16 port);
    bool nextClientId();
    bool isAvNextClient();
    bool nextPort();
    bool isPortAv();
    void resolveLogin(QByteArray *content, QHostAddress address, qint64 timeStamp);
    void newChannel(Datagram, QHostAddress address, qint64 timeStamp);

private slots:
    void readPendingDatagrams();

signals:
    void newChannelAdded(ChannelInfo);
    void channelClosed(qint32);
    void newClientConnected(ClientInfo*);
    void clientConnectionAck(qint32);
    void clientDisconnected(qint32);

};

#endif // MANAGECLIENTS_H
