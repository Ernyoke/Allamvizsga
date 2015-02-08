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
             switch(client->getClientType()) {
             case ClientInfo::SERVER : {
                 return "Server";
                 break;
             }
             case ClientInfo::LISTENER :
                 return "Listener";
                 break;
             case ClientInfo::SPEAKER : {
                 return "Speaker";
                 break;
             }
             case ClientInfo::TRANSLATOR : {
                 return "Translator";
                 break;
             }
             default:
                 return "Unknown";
                 break;
             }

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

QVariant TableModel::headerData(int section, Qt::Orientation orientation, int role) const {
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



