#include "logindialog.h"
#include "ui_logindialog.h"

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    connect(ui->loginBtn, SIGNAL(clicked()), this, SLOT(authentificate()));
}

void LoginDialog::init() {
    ack = false;
    ui->loginBtn->setText("Connect");
    ui->status->setText("-");
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::authentificate() {
    if(!ack) {
        if(Settings::checkIpAddress(ui->address->text())) {
            emit sendLoginRequest(ui->address->text());
            //set status text
            ui->status->setText("Waiting for server response!");
        }
        else {
            ui->status->setText("Invalid IPv4 address!");
        }
    }
    else {
        if(ack) {
            this->close();
        }
    }
}

void LoginDialog::authentificationSucces(qint32 id) {
    if(id > 0) {
        ui->status->setText("Authentification succes!");
        ui->loginBtn->setText("Countinue");
        ack = true;
//        settings->setClientId(id);
    }
    else {
        authentificationFailed();
    }
}

void LoginDialog::authentificationFailed() {
    ack = false;
    ui->loginBtn->setText("Retry");
    ui->loginBtn->setEnabled(true);
    ui->status->setText("Authentification failed!");
}

void LoginDialog::authentificationTimedOut() {
    if(ack == false) {
        ui->loginBtn->setText("Retry");
        ui->loginBtn->setEnabled(true);
        ui->status->setText("Server timed out!");
    }
}

bool LoginDialog::authentificationStatus() {
    return this->ack;
}

void LoginDialog::logout() {
    if(ack) {
        emit sendLogoutRequest();
    }
}
