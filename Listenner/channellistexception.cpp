#include "channellistexception.h"

ChannelListException::ChannelListException(BaseException *parent) : BaseException(parent)
{
    qDebug() << "Exception created!";
}

ChannelListException::~ChannelListException()
{
    qDebug() << "Exception distroyed!";
}


