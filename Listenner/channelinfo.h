#ifndef CHANNELINFO_H
#define CHANNELINFO_H

#include <QtGlobal>
#include <QByteArray>
#include <QString>
#include <QDataStream>

class ChannelInfo
{
public:
    ChannelInfo();
    ChannelInfo(qint32 userId, QString language, QString codec, qint32 sample_rate, qint32 sample_size, qint32 channels);
    ChannelInfo(QByteArray &content);
    ChannelInfo(const ChannelInfo *info);
    ChannelInfo(const ChannelInfo &info);
    ~ChannelInfo();

    QByteArray serialize();

    QString getLanguage() const;
    QString getCodec() const;
    qint32 getSampleRate() const;
    qint32 getSampleSize() const;
    qint32 getChannels() const;

    void setOutPort(qint32 port);
    qint32 getOwner() const;
    qint16 getOutPort() const;

private:
    QString language;
    QString codec;
    qint32 sample_rate;
    qint32 channels;
    qint32 sample_size;
    qint32 ownerId;
    qint16 port;

    void deserialize(QByteArray &content);

};

#endif // CHANNELINFO_H
