#include "channelmodel.h"

ChannelModel::ChannelModel(QObject *parent) : QAbstractTableModel(parent)
{

}

ChannelModel::~ChannelModel()
{

}

int ChannelModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return channelList.size();
}

int ChannelModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 1;
}

QVariant ChannelModel::data(const QModelIndex &index, int role) const
 {
     if (!index.isValid())
         return QVariant();

     if (index.row() >= channelList.size() || index.row() < 0)
         return QVariant();

     if (role == Qt::DisplayRole) {
         QSharedPointer<ChannelInfo> chInfo = channelList.at(index.row());
         return chInfo->getLanguage();
     }
     return QVariant();
 }

QSharedPointer<ChannelInfo> ChannelModel::getData(const QModelIndex &index) const {
    if(index.isValid()) {
        QSharedPointer<ChannelInfo> channel = channelList.at(index.row());
        return channel;
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

