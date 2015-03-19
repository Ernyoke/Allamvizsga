#ifndef CHANNELLISTEXCEPTION_H
#define CHANNELLISTEXCEPTION_H

#include <QObject>
#include <QDebug>
#include "baseexception.h"

class ChannelListException : public BaseException
{
    Q_OBJECT
public:
    explicit ChannelListException(QObject *parent = 0);
    ~ChannelListException();

signals:

public slots:
};

#endif // CHANNELLISTEXCEPTION_H
