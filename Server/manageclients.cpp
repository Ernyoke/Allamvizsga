#include "manageclients.h"
#include "ui_manageclients.h"

const int PORT = 10000;
const int MAXCLIENTS = 1024;

ManageClients::ManageClients(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ManageClients)
{
    ui->setupUi(this);

    this->socket = new QUdpSocket(this);
    socket->bind(PORT);

    clientID = 0;

    connect(socket, SIGNAL(readyRead()), this, SLOT(readPendingDatagrams()));

    model = new TableModel(this);
    ui->clientView->setModel(model);
}

ManageClients::~ManageClients()
{
    delete socket;
    delete ui;
}

void ManageClients::readPendingDatagrams() {
    while(socket->hasPendingDatagrams()) {
        QByteArray dataReceived;
        dataReceived.resize(socket->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;
        socket->readDatagram(dataReceived.data(), dataReceived.size(), &sender, &senderPort);
        Datagram dgram(&dataReceived);
        this->processDatagram(dgram, sender, senderPort);
    }
}

void ManageClients::processDatagram(Datagram dgram, QHostAddress address, quint16 port) {
    switch(dgram.getId()) {
    case Datagram::LOGIN : {
        if(isAvNextClient()) {
            QByteArray *content = dgram.getContent();
            //deserialize login data
            QDataStream out(content, QIODevice::ReadOnly);
            QString osName;
            quint32 clientType;
            out >> clientType;
            out >> osName;
            ClientInfo *info =  new ClientInfo(address, port, clientType, osName, 0);
            //clientList.insert(clientID, info);
            model->addClient(info);
            Datagram response(Datagram::LOGIN_ACK, 0, dgram.getTimeStamp());
            //insert client id into a buffer
            QByteArray toSend;
            QDataStream in(&toSend, QIODevice::WriteOnly);
            //check if new id can be generated, otherwise return error
            if(nextClientId()) {
                in << clientID;
                info->setId(clientID);
            }
            else {
                in << 0;
            }
            response.setDatagramContent(&toSend);
            response.sendDatagram(socket, &address, 40000);
        }
        break;
    }
    case Datagram::LOGIN_ACK : {
        model->setAck(dgram.getClientId());
        break;
    }
    case Datagram::LOGOUT : {
        model->removeClient(dgram.getClientId());
        break;
    }
    default : {

        break;
    }
    }
}

bool ManageClients::nextClientId() {
    if(model->rowCount(QModelIndex()) >= MAXCLIENTS) {
        return false;
    }
    if(clientID + 1 >= MAXCLIENTS) {
        clientID = 0;
    }
    else {
        clientID += 1;
    }
    while (model->containsClient(clientID)) {
        if(clientID + 1 >= MAXCLIENTS) {
            clientID = 0;
        }
        else {
            clientID += 1;
        }
    }
    return true;
}

bool ManageClients::isAvNextClient() {
    if(model->rowCount(QModelIndex()) < MAXCLIENTS) {
        return true;
    }
    return false;
}


