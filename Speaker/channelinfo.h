#ifndef CHANNELINFO_H
#define CHANNELINFO_H

#include <QtGlobal>
#include <QByteArray>
#include <QString>
#include <QDataStream>

class ChannelInfo
{
public:
    ChannelInfo(qint32 userId, QString language, QString codec, qint32 sample_rate, qint32 sample_size, qint32 channels);
    ChannelInfo(QByteArray *content);
    ChannelInfo(ChannelInfo *info);
    ChannelInfo(ChannelInfo &info);
    ~ChannelInfo();

    QByteArray serialize();

    QString getLanguage() const;
    QString getCodec() const;
    qint32 getSampleRate() const;
    qint32 getSampleSize() const;
    qint32 getChannels() const;

    void setOutPort(qint16 port);
    qint32 getOwner();
    qint16 getOutPort();

private:
    QString language;
    QString codec;
    qint32 sample_rate;
    qint32 channels;
    qint32 sample_size;
    qint32 ownerId;
    qint16 port;

    void deserialize(QByteArray *content);

};

#endif // CHANNELINFO_H
