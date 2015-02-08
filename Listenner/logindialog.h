#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QUdpSocket>
#include <QSysInfo>
#include <QDebug>
#include <QTimer>
#include <QDateTime>

#include "datagram.h"
#include "settings.h"

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    LoginDialog(Settings*, QWidget *parent = 0);
    ~LoginDialog();

    bool loginSucces();

private:
    Ui::LoginDialog *ui;

    Settings *settings;
    QUdpSocket *socket;

    bool ack;

    Datagram *dgram;

    QTimer *timer;

    void processDatagram(Datagram);
    qint64 generateTimestamp();

private slots:
    void authentificate();
    void readDatagram();
    void loginTimedOut();
    void logout();
};

#endif // LOGINDIALOG_H
