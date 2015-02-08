#include "logindialog.h"
#include "ui_logindialog.h"

LoginDialog::LoginDialog(Settings *settings, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    this->settings = settings;

    socket = new QUdpSocket(this);
    ack = false;

    connect(socket, SIGNAL(readyRead()), this, SLOT(readDatagram()));

    timer = new QTimer(this);
    timer->setSingleShot(true);
    connect(timer, SIGNAL(timeout()), this, SLOT(loginTimedOut()));
    connect(ui->loginBtn, SIGNAL(clicked()), this, SLOT(authentificate()));

    dgram = NULL;

}

LoginDialog::~LoginDialog()
{
    delete dgram;
    delete ui;
}

void LoginDialog::authentificate() {
    if(!ack) {
        if(settings->setServerAddress(ui->address->text())) {
            //bind the socket to the server address
            socket->bind(*settings->getServerAddress(), settings->getClientPort());
            //get the current timespamp;
            qint64 timeStamp = generateTimestamp();
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
    }
    else {
        if(ack) {
            this->close();
        }
    }
}

void LoginDialog::readDatagram() {
    while(socket->hasPendingDatagrams()) {
        QByteArray dataReceived;
        dataReceived.resize(socket->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;
        socket->readDatagram(dataReceived.data(), dataReceived.size(), &sender, &senderPort);
        Datagram dgram(&dataReceived);
        this->processDatagram(dgram);
    }
}

void LoginDialog::loginTimedOut() {
    if(ack == false) {
        ui->loginBtn->setText("Retry");
        ui->loginBtn->setEnabled(true);
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
            ui->status->setText("Authentification succes!");
            ui->loginBtn->setText("Countinue");
            timer->stop();
            QString respContent = " ";
            Datagram response(Datagram::LOGIN_ACK, settings->getClientId(), generateTimestamp(), &respContent);
            response.sendDatagram(this->socket, settings->getServerAddress(), settings->getServerPort());
        }
        else {
            ui->status->setText("Authentification failed!");
            ui->loginBtn->setText("Retry");
        }
        qDebug() << id;
    }
}

bool LoginDialog::loginSucces() {
    return this->ack;
}

qint64 LoginDialog::generateTimestamp() {
    QDateTime now = QDateTime::currentDateTime();
    qint64 timeStamp = now.currentDateTime().toMSecsSinceEpoch();
    return timeStamp;
}

void LoginDialog::logout() {
    if(ack) {
        QString respContent = " ";
        Datagram response(Datagram::LOGOUT, settings->getClientId(), generateTimestamp(), &respContent);
        response.sendDatagram(this->socket, settings->getServerAddress(), settings->getServerPort());
    }
}
