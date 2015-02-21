#ifndef CLIENTINFO_H
#define CLIENTINFO_H

#include <QHostAddress>

class ClientInfo
{
public:

    enum CLIENT_TYPE {
        SERVER = 0,
        SPEAKER = 1,
        LISTENER = 2,
        TRANSLATOR = 3
    };

    ClientInfo(QHostAddress address, QString OSName, double version);
    ~ClientInfo();

    void setId(quint32 id);
    quint32 getId();
    QString getAddressStr();
    QHostAddress getAddress();
    QString getOSName();
    bool active();
    void setAck();

    virtual CLIENT_TYPE getClientType() const = 0;
    virtual QString getClientTypeStr() const = 0;

protected:
    quint32 id;
    QHostAddress address;
    QString OSName;
    double version;
    bool isActive;
};

#endif // CLIENTINFO_H
