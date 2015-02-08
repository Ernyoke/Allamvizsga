#ifndef TABLEMODEL_H
#define TABLEMODEL_H

#include <QAbstractTableModel>
#include <QVariant>
#include <QVector>
#include <QVectorIterator>

#include "clientinfo.h"

class TableModel : public QAbstractTableModel
{
    Q_OBJECT

private:
    QVector<ClientInfo*> clientList;

public:
    TableModel(QObject *parent = 0);
    ~TableModel();

    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    void addClient(ClientInfo *client);
    void removeClient(quint32 id);
    bool containsClient(quint32 id);
    void setAck(quint32 clientId);
};

#endif // TABLEMODEL_H
