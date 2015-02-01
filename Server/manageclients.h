#ifndef MANAGECLIENTS_H
#define MANAGECLIENTS_H

#include <QObject>
#include <QUdpSocket>
#include <QMap>

#include "clientinfo.h"
#include "datagram.h"

class ManageClients : public QObject
{
    Q_OBJECT
public:
    ManageClients();
    ~ManageClients();

private:
    QUdpSocket *socket;
    QMap<int, ClientInfo*> clientList;

    qint32 clientID;

    void processDatagram(Datagram, QHostAddress address, quint16 port);
    bool nextClientId();

private slots:
    void readPendingDatagrams();

};

#endif // MANAGECLIENTS_H
