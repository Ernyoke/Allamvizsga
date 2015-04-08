#include "logindialog.h"
#include "ui_logindialog.h"

LoginDialog::LoginDialog(Settings *settings, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    this->settings = settings;
    connect(ui->loginBtn, SIGNAL(clicked()), this, SLOT(authentificate()));
}

void LoginDialog::init() {
    ack = false;
    ui->loginBtn->setText("Retry");
    ui->status->setText("-");
}

LoginDialog::~LoginDialog()
{
//    delete dgram;
    delete ui;
}

void LoginDialog::authentificate() {
    if(!ack) {
        if(settings->setServerAddress(ui->address->text())) {
            emit sendLoginRequest();
            //set status text
            ui->status->setText("Waiting for server response!");
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
        settings->setClientId(id);
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
