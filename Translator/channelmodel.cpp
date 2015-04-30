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
            return QString("%1 (Port: %2, %3 Hz, %4 bits)").arg(chInfo->getLanguage()).arg(chInfo->getOutPort())
                    .arg(chInfo->getSampleRate()).arg(chInfo->getSampleSize());
         }
         else {
             QSharedPointer<ChannelInfo> chInfo = userCreatedChannelList.at(index.row() - channelList.size());
             return QString("%1 (Port: %2, %3 Hz, %4 bits)").arg(chInfo->getLanguage()).arg(chInfo->getOutPort())
                     .arg(chInfo->getSampleRate()).arg(chInfo->getSampleSize());
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
}


void ChannelModel::deleteUserCreatedChannel(QModelIndex index) {
    if(index.isValid()) {
        int row = index.row() - channelList.size();
        if(row >= 0) {
            beginRemoveRows(index, index.row(), index.row());
            userCreatedChannelList.remove(row);
            endRemoveRows();
        }
        else {
            emit error("Channel can not be removed!");
        }
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

