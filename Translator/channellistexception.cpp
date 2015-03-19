#include "channellistexception.h"

ChannelListException::ChannelListException(QObject *parent) : BaseException(parent)
{
    qDebug() << "ChannelListException created!";
}

ChannelListException::~ChannelListException()
{
    qDebug() << "ChannelListException distroyed!";
}


