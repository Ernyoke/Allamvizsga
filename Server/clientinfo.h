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

    ClientInfo(QHostAddress address, qint32 port, QString OSName, double version);
    virtual ~ClientInfo();

    void setId(quint32 id);
    quint32 getId() const;
    QString getAddressStr() const;
    QHostAddress getAddress() const;
    qint32 getClientPort()  const;
    QString getOSName() const;
    bool active() const;
    void setAck();
    void incNoResponseCounter();
    void resetNoResponseCounter();
    bool isOnline() const;

    virtual CLIENT_TYPE getClientType() const = 0;
    virtual QString getClientTypeStr() const = 0;

protected:
    quint32 id;
    QHostAddress address;
    qint32 clientPort;
    QString OSName;
    double version;
    bool isActive;

    int noResponseCounter;

};

#endif // CLIENTINFO_H
