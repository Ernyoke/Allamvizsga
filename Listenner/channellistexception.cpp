#include "channellistexception.h"

ChannelListException::ChannelListException(QObject *parent) : QObject(parent)
{
    qDebug() << "Exception created!";
}

ChannelListException::~ChannelListException()
{
    qDebug() << "Exception distroyed!";
}

QString ChannelListException::message() {
    return this->msg;
}

void ChannelListException::setMessage(QString msg) {
    this->msg = msg;
}

