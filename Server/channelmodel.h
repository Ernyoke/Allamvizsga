#ifndef CHANNELMODEL_H
#define CHANNELMODEL_H

#include <QObject>
#include <QAbstractTableModel>
#include <QMap>
#include <QVector>
#include <QDebug>
#include <QSharedPointer>
#include <QPair>

#include "channelinfo.h"
#include "channelnotfoundex.h"

class ChannelModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit ChannelModel(QObject *parent = 0);
    ~ChannelModel();

    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    bool hasPortAssigned(qint16 port) const;
    QByteArray serialize();
    QByteArray serializeChannel(qint32 id);
    QPair<bool, int> containsChannel(qint32 id) const;

private:
    QVector< QSharedPointer<ChannelInfo> > channelList;

signals:

public slots:
    void addNewChannel(ChannelInfo);
    void deleteChannel(qint32 id);

};

#endif // CHANNELMODEL_H
