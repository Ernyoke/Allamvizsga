#ifndef CLIENTINFO_H
#define CLIENTINFO_H

#include <QHostAddress>

class ClientInfo
{
public:
    ClientInfo(QHostAddress address, quint16 port, quint32 clientType, QString OSName, double version);
    ~ClientInfo();

    void setId(quint32 id);
    quint32 getId();
    quint32 getClientType();
    QString getAddressStr();
    QString getOSName();
    bool active();
    void setAck();

private:
    quint32 id;
    quint32 clientType;
    QHostAddress address;
    QString OSName;
    double version;
    bool isActive;
    quint16 port;
};

#endif // CLIENTINFO_H
