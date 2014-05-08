#ifndef CONNECTTOSERVER_H
#define CONNECTTOSERVER_H

#include <QObject>
#include <QTimer>
#include <QUdpSocket>
#include <QBuffer>
#include "gui.h"

class connectToServer : public QObject
{
    Q_OBJECT
public:
    explicit connectToServer(QUdpSocket *socket, GUI *gui , QObject *parent = 0);
    bool conn();
    int getID();

private:
    QUdpSocket *socket;
    GUI *gui;
    QByteArray *sendBuffer;
    int threadID;


signals:

public slots:
    void checkDatagramm();

};

#endif // CONNECTTOSERVER_H
