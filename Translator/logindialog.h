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
    LoginDialog(Settings *settings, QWidget *parent = 0);
    ~LoginDialog();

    bool loginSucces();

private:
    Ui::LoginDialog *ui;

    Settings *settings;

    bool ack;

    QTimer *timer;

private slots:
    void authentificate();
    void processLogin(Datagram dgram);
    void loginTimedOut();
    void logout();

signals:
    void sendLoginRequest(Datagram dgram);
    void sendLogoutRequest(Datagram dgram);
    void sendLoginResponse(Datagram dgram);
};

#endif // LOGINDIALOG_H
