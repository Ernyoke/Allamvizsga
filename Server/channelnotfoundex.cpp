#include "channelnotfoundex.h"

ChannelNotFoundEx::ChannelNotFoundEx(QString &msg, QObject *parent) : QObject(parent)
{
    this->msg = msg;
}

ChannelNotFoundEx::~ChannelNotFoundEx()
{

}

QString ChannelNotFoundEx::message() {
    return this->msg;
}

void ChannelNotFoundEx::setMessage(QString& msg) {
    this->msg = msg;
}

