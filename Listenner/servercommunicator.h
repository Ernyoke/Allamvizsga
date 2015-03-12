#ifndef SERVERCOMMUNICATOR_H
#define SERVERCOMMUNICATOR_H

#include <QObject>
#include <QUdpSocket>
#include <QTimer>
#include <QMap>

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
    QTimer *listReqTimer;

    qint64 reqListStart;
    QMap<qint32, QByteArray> listContent;

    void processDatagram(Datagram);
    void processList(Datagram&);
    void processSynch(Datagram&);

private slots:
    void sendLoginRequest(Datagram);
    void sendDatagram(Datagram);
    void readDatagram();
    void listReqTimedOut();

public slots:
    void requestChannelList();

signals:
    void loginAckReceived(Datagram);
    void serverList(QByteArray);
//    void newChannelAckReceived(Datagram);

public slots:
};

#endif // SERVERCOMMUNICATOR_H
