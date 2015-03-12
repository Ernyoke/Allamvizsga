#include "channelmodel.h"

ChannelModel::ChannelModel(QObject *parent) : QAbstractListModel(parent)
{

}

ChannelModel::~ChannelModel()
{

}

int ChannelModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return channelList.size() + userCreatedChannelList.size();
}

QVariant ChannelModel::data(const QModelIndex &index, int role) const
 {
     if (!index.isValid())
         return QVariant();

     if (index.row() >= channelList.size() + userCreatedChannelList.size() || index.row() < 0)
         return QVariant();

     if (role == Qt::DisplayRole) {
         if(index.row() < channelList.size()) {
            QSharedPointer<ChannelInfo> chInfo = channelList.at(index.row());
            return chInfo->getLanguage();
         }
         else {
             QSharedPointer<ChannelInfo> chInfo = userCreatedChannelList.at(index.row() - channelList.size());
             return chInfo->getLanguage();
         }
     }
     return QVariant();
 }

QSharedPointer<ChannelInfo> ChannelModel::getData(const QModelIndex &index) const {
    if(index.isValid()) {
        if(index.row() < channelList.size()) {
           QSharedPointer<ChannelInfo> chInfo = channelList.at(index.row());
           return chInfo;
        }
        else {
            QSharedPointer<ChannelInfo> chInfo = userCreatedChannelList.at(index.row() - channelList.size());
            return chInfo;
        }
    }
    ChannelListException *exception = new ChannelListException;
    exception->setMessage("Invalid index!");
    throw exception;
}


void ChannelModel::addNewChannel(ChannelInfo info) {
    int position = this->rowCount(QModelIndex());
    beginInsertRows(QModelIndex(), position, position);
    QSharedPointer<ChannelInfo> chInfo = QSharedPointer<ChannelInfo>(new ChannelInfo(info));
    channelList.append(chInfo);
    endInsertRows();
    qDebug() << info.getLanguage();
}

void ChannelModel::deleteChannel(qint32 id) {
    QVectorIterator< QSharedPointer<ChannelInfo> > iter(channelList);
    int i = 0;
    while(iter.hasNext()) {
        QSharedPointer<ChannelInfo> tempInfo = iter.next();
        if(tempInfo->getOwner() == id) {
            beginRemoveRows(QModelIndex(), i, i);
            channelList.remove(i);
            endRemoveRows();
            break;
        }
        ++i;
    }
}

void ChannelModel::addNewUserCreatedChannel(ChannelInfo info) {
    int position = this->rowCount(QModelIndex());
    beginInsertRows(QModelIndex(), position, position);
    QSharedPointer<ChannelInfo> chInfo = QSharedPointer<ChannelInfo>(new ChannelInfo(info));
    userCreatedChannelList.append(chInfo);
    endInsertRows();
    qDebug() << info.getLanguage();
}


void ChannelModel::deleteUserCreatedChannel(qint32 id) {
    QVectorIterator< QSharedPointer<ChannelInfo> > iter(userCreatedChannelList);
    int i = 0;
    while(iter.hasNext()) {
        QSharedPointer<ChannelInfo> tempInfo = iter.next();
        if(tempInfo->getOwner() == id) {
            beginRemoveRows(QModelIndex(), i, i);
            userCreatedChannelList.remove(i);
            endRemoveRows();
            break;
        }
        ++i;
    }
}


void ChannelModel::newChannelList(QByteArray listBuffer) {
    channelList.clear();
    QDataStream out(&listBuffer, QIODevice::ReadOnly);
    qint32 listSize;
    out >> listSize;
    QByteArray channelInfoSerialized;
    int position = this->rowCount(QModelIndex());
    beginInsertRows(QModelIndex(), position, position);
    for(int i = 0; i < listSize; ++i) {
        out >> channelInfoSerialized;
         QSharedPointer<ChannelInfo> chInfo = QSharedPointer<ChannelInfo>(new ChannelInfo(channelInfoSerialized));
         channelList.append(chInfo);
    }
    endInsertRows();
}
