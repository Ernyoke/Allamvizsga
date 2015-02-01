#include "logindialog.h"
#include "ui_logindialog.h"

LoginDialog::LoginDialog(Settings *settings, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    this->settings = settings;

    socket = new QUdpSocket(this);
    socket->bind(*settings->getServerAddress(), settings->getClientPort());
    ack = false;

    connect(socket, SIGNAL(readyRead()), this, SLOT(ackLogin()));
    connect(ui->okBtn, SIGNAL(clicked()), this, SLOT(retryLogin()));

    timer = new QTimer(this);
    timer->setSingleShot(true);
    connect(timer, SIGNAL(timeout()), this, SLOT(loginTimedOut()));

}

LoginDialog::~LoginDialog()
{
    delete dgram;
    delete ui;
}

void LoginDialog::login() {
    //get the current timespamp;
    QDateTime now = QDateTime::currentDateTime();
    qint64 timeStamp = now.currentDateTime().toMSecsSinceEpoch();
    //get system information
    QSysInfo sysInfo;
    QString os = sysInfo.prettyProductName();
    //create buffer which needs to be sent
    //4 byte SPEAKER_ID, remaining bytes os info
    QByteArray content;
    QDataStream in(&content, QIODevice::WriteOnly);
    in << settings->getClientType();
    in << os;
    //create the datagram
    dgram = new Datagram(Datagram::LOGIN, settings->getClientId(), timeStamp);
    dgram->setDatagramContent(&content);
    //send the package to the server
    dgram->sendDatagram(socket, settings->getServerAddress(), settings->getServerPort());
    //set a timer for response
    timer->setInterval(1000);
    timer->start();
    //set status text
    ui->status->setText("Waiting for server response!");
}

void LoginDialog::retryLogin() {
    dgram->sendDatagram(socket, settings->getServerAddress(), settings->getServerPort());
    QTimer::singleShot(1000, this, SLOT(loginTimedOut()));
}

void LoginDialog::ackLogin() {
    while(socket->hasPendingDatagrams()) {
        QByteArray dataReceived;
        dataReceived.resize(socket->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;
        socket->readDatagram(dataReceived.data(), dataReceived.size(), &sender, &senderPort);
        Datagram dgram(&dataReceived);
        this->processDatagram(dgram);
    }
    //dgram->sendDatagram(socket, address, port);
}

void LoginDialog::loginTimedOut() {
    if(ack == false) {
        ui->okBtn->setText("Retry");
        ui->okBtn->setEnabled(true);
        ui->status->setText("Server timed out!");
    }
}

void LoginDialog::processDatagram(Datagram dgram) {
    if(dgram.getId() == Datagram::LOGIN_ACK) {
        QByteArray *content = dgram.getContent();
        QDataStream out(content, QIODevice::ReadOnly);
        qint32 id;
        out >> id;
        settings->setClientId(id);
        ack = true;
        if(id > 0) {
            ui->status->setText("Logged in!");
            timer->stop();
        }
        else {
            ui->status->setText("error logging in");
        }
        qDebug() << id;
    }
}

bool LoginDialog::loginSucces() {
    return this->ack;
}
