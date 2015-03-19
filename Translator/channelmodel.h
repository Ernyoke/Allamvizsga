#ifndef CHANNELMODEL_H
#define CHANNELMODEL_H

#include <QObject>
#include <QAbstractListModel>
#include <QMap>
#include <QVector>
#include <QDebug>
#include <QSharedPointer>

#include "channelinfo.h"
#include "channellistexception.h"

class ChannelModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit ChannelModel(QObject *parent = 0);
    ~ChannelModel();

    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QSharedPointer<ChannelInfo> getData(const QModelIndex &index) const;

private:
    QVector< QSharedPointer<ChannelInfo> > channelList;
    QVector< QSharedPointer<ChannelInfo> > userCreatedChannelList;

signals:
    void error(QString);

public slots:
    void addNewChannel(ChannelInfo);
    void deleteChannel(qint32 id);
    void newChannelList(QByteArray listBuffer);
    void addNewUserCreatedChannel(ChannelInfo info);
    void deleteUserCreatedChannel(QModelIndex);

};

#endif // CHANNELMODEL_H
