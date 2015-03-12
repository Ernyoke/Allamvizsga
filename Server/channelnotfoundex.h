#ifndef CHANNELNOTFOUNDEX_H
#define CHANNELNOTFOUNDEX_H

#include <QObject>

class ChannelNotFoundEx : public QObject
{
    Q_OBJECT
public:
    explicit ChannelNotFoundEx(QString &msg, QObject *parent = 0);
    ~ChannelNotFoundEx();

    QString message();
    void setMessage(QString&);

private:
    QString msg;

signals:

public slots:
};

#endif // CHANNELNOTFOUNDEX_H
