#include "tablemodel.h"

TableModel::TableModel(QObject *parent) : QAbstractTableModel(parent)
{

}

TableModel::~TableModel()
{
    //delete all clients
}

int TableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return clientList.size();
}

int TableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 5;
}

QVariant TableModel::data(const QModelIndex &index, int role) const
 {
     if (!index.isValid())
         return QVariant();

     if (index.row() >= clientList.size() || index.row() < 0)
         return QVariant();

     if (role == Qt::DisplayRole) {
         ClientInfo *client = clientList.at(index.row());

         switch (index.column()) {
         case 0:
             return client->getId();
             break;
         case 1:
             return client->getAddressStr();
             break;
         case 2:
             return client->getClientType();
             break;
         case 3:
             return client->getOSName();
             break;
         case 4:
             if(client->active()) {
                 return "acknowledged";
             }
             else {
                 return "waiting for ack";
             }
         default:
             break;
         }
     }
     return QVariant();
 }

void TableModel::addClient(ClientInfo *client) {
    int position = this->rowCount(QModelIndex());
    beginInsertRows(QModelIndex(), position, position + 1);
    clientList.append(client);
    endInsertRows();
}

void TableModel::removeClient(quint32 id) {
    QVectorIterator<ClientInfo*> iter(clientList);
    int i = 0;
    while(iter.hasNext()) {
        ClientInfo *client = iter.next();
        if(client->getId() == id) {
            beginRemoveRows(QModelIndex(), i, i + 1);
            clientList.remove(i);
            endRemoveRows();
            break;
        }
        ++i;
    }
}

bool TableModel::containsClient(quint32 id) {
    QVectorIterator<ClientInfo*> iter(clientList);
    while(iter.hasNext()) {
        ClientInfo *client = iter.next();
        if(client->getId() == id) {
            return true;
        }
    }
    return false;
}

void TableModel::setAck(quint32 clientId) {
    QVectorIterator<ClientInfo*> iter(clientList);
    while(iter.hasNext()) {
        ClientInfo *client = iter.next();
        if(client->getId() == clientId) {
            client->setAck();
        }
    }
}



