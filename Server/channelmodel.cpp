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
    return 7;
}

QVariant ChannelModel::data(const QModelIndex &index, int role) const
 {
     if (!index.isValid())
         return QVariant();

     if (index.row() >= channelList.size() || index.row() < 0)
         return QVariant();

     if (role == Qt::DisplayRole) {
         QSharedPointer<ChannelInfo> info = channelList.at(index.row());
         switch (index.column()) {
         case 0: {
             return QString::number(info->getOutPort());
             break;
         }
         case 1: {
             return QString::number(info->getOwner());
             break;
         }
         case 2: {
             return info->getLanguage();
             break;
         }
         case 3: {
             return info->getCodec();
             break;
         }
         case 4: {
             return QString("%1 Hz").arg(info->getSampleRate());
             break;
         }
         case 5: {
             return QString::number(info->getSampleSize());
             break;
         }
         case 6: {
            return QString::number(info->getChannels());
            break;
         }
         default:
             return QVariant();
             break;
         }
     }
     return QVariant();
 }

QVariant ChannelModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role != Qt::DisplayRole)
             return QVariant();
    if (orientation == Qt::Horizontal) {
             switch (section) {
             case 0:
                 return tr("Port");
                 break;
             case 1:
                 return tr("Speaker ID");
                 break;
             case 2:
                 return tr("Language");
                 break;
             case 3:
                 return tr("Codec");
                 break;
             case 4:
                 return tr("Sample Rate");
                 break;
             case 5:
                 return tr("Sample Size");
                 break;
             case 6:
                 return tr("Channels");
                 break;
             default:
                 return QVariant();
         }
    }
    return QVariant();
}

void ChannelModel::addNewChannel(ChannelInfo info) {
    int position = this->rowCount(QModelIndex());
    QPair<bool, int> contains = containsChannel(info.getOwner());
    if(contains.first) {
        beginRemoveRows(QModelIndex(), contains.second, contains.second);
        channelList.remove(contains.second);
        endRemoveRows();
    }
    QSharedPointer<ChannelInfo> channelInfo = QSharedPointer<ChannelInfo>(new ChannelInfo(info));
    beginInsertRows(QModelIndex(), position, position);
    channelList.append(channelInfo);
    endInsertRows();
    qDebug() << info.getLanguage();
}

QPair<bool, qint32> ChannelModel::containsChannel(qint32 id) const {
    QVectorIterator< QSharedPointer<ChannelInfo> > iter(channelList);
    int i = 0;
    while(iter.hasNext()) {
        QSharedPointer<ChannelInfo> tempInfo = iter.next();
        if(tempInfo->getOwner() == id) {
            return qMakePair<bool, qint32>(true, i);
        }
        ++i;
    }
    return qMakePair<bool, qint32>(false, i);
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

bool ChannelModel::hasPortAssigned(qint16 port) const {
    QVectorIterator< QSharedPointer<ChannelInfo> > iter(channelList);
    while(iter.hasNext()) {
        QSharedPointer<ChannelInfo> tempInfo = iter.next();
        if(tempInfo->getOutPort() == port) {
            return true;
        }
    }
    return false;
}

QByteArray ChannelModel::serialize() {
    QByteArray buffer;
    QDataStream out(&buffer, QIODevice::WriteOnly);
    //how many channels are online
    out << channelList.size();
    //serialize all channels
    QVectorIterator< QSharedPointer<ChannelInfo> > iter(channelList);
    while(iter.hasNext()) {
        QSharedPointer<ChannelInfo> tempInfo = iter.next();
        QByteArray content = tempInfo->serialize();
        out << content;
    }
    return buffer;
}

QByteArray ChannelModel::serializeChannel(qint32 id) {
    QPair<bool, int> contains = containsChannel(id);
    if(contains.first) {
        return channelList.at(contains.second)->serialize();
    }
    else {
        QString message("Channel not found!");
        throw new ChannelNotFoundEx(message);
    }
}
