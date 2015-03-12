#ifndef CLIENTMODEL_H
#define CLIENTMODEL_H

#include <QAbstractTableModel>
#include <QVariant>
#include <QVector>
#include <QVectorIterator>
#include <QSharedPointer>

#include "clientinfo.h"
#include "listenerclientinfo.h"
#include "speakerclientinfo.h"

class ClientModel : public QAbstractTableModel
{
    Q_OBJECT

private:
    QVector< QSharedPointer<ClientInfo> > clientList;

public:
    ClientModel(QObject *parent = 0);
    ~ClientModel();

    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    bool containsClient(qint32 id);

    QVector< QSharedPointer<ClientInfo > > getClientList() const;
    void removeOfflineClients();
    QSharedPointer<ClientInfo> getClientWithId(qint32) const;

public slots:
    void addClient(ClientInfo *client);
//    void clientAck(qint32 id);
    void removeClient(qint32 id);
    void setAck(qint32 clientId);

signals:
    void clientTimedOut(qint32);

};

#endif // CLIENTMODEL_H
