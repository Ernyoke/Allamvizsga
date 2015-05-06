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
    LoginDialog(QWidget *parent = 0);
    ~LoginDialog();

    bool authentificationStatus();
    void init();

private:
    Ui::LoginDialog *ui;

    bool ack;

private slots:
    void authentificate();
    void authentificationSucces(qint32);
    void authentificationFailed();
    void authentificationTimedOut();
    void logout();

signals:
    void sendLoginRequest(QString);
    void sendLogoutRequest();
};

#endif // LOGINDIALOG_H
