#include "clientmodel.h"

ClientModel::ClientModel(QObject *parent) : QAbstractTableModel(parent)
{

}

ClientModel::~ClientModel()
{
    //delete all clients
//    QVectorIterator<ClientInfo*> iter(clientList);
//    while(iter.hasNext()) {
//        ClientInfo *client = iter.next();
//        clientList.remove(0);
//        delete client;
//    }
    clientList.clear();
    clientMap.clear();
}

int ClientModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return clientList.size();
}

int ClientModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 5;
}

QVariant ClientModel::data(const QModelIndex &index, int role) const
 {
     if (!index.isValid())
         return QVariant();

     if (index.row() >= clientList.size() || index.row() < 0)
         return QVariant();

     if (role == Qt::DisplayRole) {
         QSharedPointer<ClientInfo> client = clientList.at(index.row());

         switch (index.column()) {
         case 0:
             return client->getId();
             break;
         case 1:
             return client->getAddressStr();
             break;
         case 2:
             return client->getClientTypeStr();

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

QVariant ClientModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role != Qt::DisplayRole)
             return QVariant();
    if (orientation == Qt::Horizontal) {
             switch (section) {
             case 0:
                 return tr("ClientID");
                 break;
             case 1:
                 return tr("Address");
                 break;
             case 2:
                 return tr("Type");
                 break;
             case 3:
                 return tr("OS");
                 break;
             case 4:
                 return tr("Status");
                 break;
             }
         }
    return QVariant();
}

void ClientModel::addClient(ClientInfo *client) {
    int position = this->rowCount(QModelIndex());
    beginInsertRows(QModelIndex(), position, position);
    QSharedPointer<ClientInfo> newClient = QSharedPointer<ClientInfo>(client);
    clientList.append(newClient);
    clientMap.insert(client->getId(), newClient);
    endInsertRows();
}

void ClientModel::removeClient(qint32 id) {
    QVectorIterator< QSharedPointer<ClientInfo> > iter(clientList);
    int i = 0;
    while(iter.hasNext()) {
        QSharedPointer<ClientInfo> client = iter.next();
        if(client->getId() == id) {
            beginRemoveRows(QModelIndex(), i, i);
            clientList.remove(i);
            clientMap.remove(id);
            endRemoveRows();
            break;
        }
        ++i;
    }
}

bool ClientModel::containsClient(qint32 id) {
    QVectorIterator< QSharedPointer<ClientInfo> > iter(clientList);
    while(iter.hasNext()) {
        QSharedPointer<ClientInfo> client = iter.next();
        if(client->getId() == id) {
            return true;
        }
    }
    return false;
}

void ClientModel::setAck(qint32 clientId) {
    QMap<qint32, QSharedPointer<ClientInfo> >::iterator iter = clientMap.find(clientId);
    if(clientMap.end() != iter) {
        QSharedPointer<ClientInfo> client = iter.value();
        client->setAck();
        QModelIndex topLeft = this->index(0, 0);
        QModelIndex bottomRight = this->index(rowCount(QModelIndex()) - 1, columnCount(QModelIndex()) - 1);
        emit dataChanged(topLeft, bottomRight);
    }
}



