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

    void login();
    bool loginSucces();

private:
    Ui::LoginDialog *ui;

    Settings *settings;
    QUdpSocket *socket;

    bool ack;

    Datagram *dgram;

    QTimer *timer;

    void processDatagram(Datagram);

private slots:
    void ackLogin();
    void loginTimedOut();
    void retryLogin();
};

#endif // LOGINDIALOG_H
