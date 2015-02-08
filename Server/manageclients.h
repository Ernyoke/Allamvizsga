#ifndef MANAGECLIENTS_H
#define MANAGECLIENTS_H

#include <QMainWindow>
#include <QUdpSocket>
#include <QMap>

#include "clientinfo.h"
#include "datagram.h"
#include "tablemodel.h"

namespace Ui {
class ManageClients;
}

class ManageClients : public QMainWindow
{
    Q_OBJECT

public:
    explicit ManageClients(QWidget *parent = 0);
    ~ManageClients();

private:
    Ui::ManageClients *ui;

    QUdpSocket *socket;

    qint32 clientID;

    TableModel *model;

    void processDatagram(Datagram, QHostAddress address, quint16 port);
    bool nextClientId();
    bool isAvNextClient();

private slots:
    void readPendingDatagrams();
};

#endif // MANAGECLIENTS_H
