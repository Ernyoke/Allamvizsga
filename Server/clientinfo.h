#ifndef CLIENTINFO_H
#define CLIENTINFO_H

#include <QHostAddress>
#include <cstdint>

class ClientInfo
{
public:
    ClientInfo(QHostAddress address, quint16 port, uint32_t clientType, QString OSName, double version);
    ~ClientInfo();

private:
    uint32_t clientType;
    QHostAddress address;
    QString OSName;
    double version;
    bool isActive;
    quint16 port;
};

#endif // CLIENTINFO_H
