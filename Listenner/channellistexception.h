#ifndef CHANNELLISTEXCEPTION_H
#define CHANNELLISTEXCEPTION_H

#include <QObject>
#include <QDebug>
#include "baseexception.h"

class ChannelListException : public BaseException
{
    Q_OBJECT
public:
    explicit ChannelListException(BaseException *parent = 0);
    ~ChannelListException();


private:
    QString msg;

signals:

public slots:
};

#endif // CHANNELLISTEXCEPTION_H
