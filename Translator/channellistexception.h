#ifndef CHANNELLISTEXCEPTION_H
#define CHANNELLISTEXCEPTION_H

#include <QObject>
#include <QDebug>

class ChannelListException : public QObject
{
    Q_OBJECT
public:
    explicit ChannelListException(QObject *parent = 0);
    ~ChannelListException();

    QString message();
    void setMessage(QString);

private:
    QString msg;

signals:

public slots:
};

#endif // CHANNELLISTEXCEPTION_H
