#include "gui.h"
#include "ui_gui.h"

GUI::GUI(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GUI)
{
    ui->setupUi(this);

    channelModel = new ChannelModel(this);
    clientModel = new ClientModel(this);

    soundWorker = new AcceptData;
    soundThread = new QThread;
    soundWorker->moveToThread(soundThread);
    connect(soundWorker, SIGNAL(finished()), soundThread, SLOT(quit()));
    connect(this, SIGNAL(stopSoundWorker()), soundWorker, SLOT(stopWorker()), Qt::DirectConnection);
    connect(soundWorker, SIGNAL(finished()), soundThread, SLOT(deleteLater()));
    connect(soundWorker, SIGNAL(finished()), soundWorker, SLOT(deleteLater()));
    soundThread->start();

    //menu handlers
    connect(ui->actionConnected_Clients, SIGNAL(triggered()), this, SLOT(showClientList()));

    ui->channelTab->setModel(channelModel);
    manageClients = new ManageClients(clientModel, channelModel, this);
    connect(manageClients, SIGNAL(newChannelAdded(ChannelInfo)), channelModel, SLOT(addNewChannel(ChannelInfo)));
    connect(manageClients, SIGNAL(newChannelAdded(ChannelInfo)), soundWorker, SLOT(addChannel(ChannelInfo)));
    connect(manageClients, SIGNAL(channelClosed(qint32)), channelModel, SLOT(deleteChannel(qint32)));
    connect(manageClients, SIGNAL(channelClosed(qint32)), soundWorker, SLOT(removeChannel(qint32)));

    showClients = new ShowClients(clientModel, this);
    connect(manageClients, SIGNAL(newClientConnected(ClientInfo*)), clientModel, SLOT(addClient(ClientInfo*)));
    connect(manageClients, SIGNAL(newClientConnected(ClientInfo*)), this, SLOT(logClientConnected(ClientInfo*)));

    connect(manageClients, SIGNAL(clientConnectionAck(qint32)), clientModel, SLOT(setAck(qint32)));
    connect(manageClients, SIGNAL(clientDisconnected(qint32)), clientModel, SLOT(removeClient(qint32)));
    connect(manageClients, SIGNAL(clientDisconnected(qint32)), this, SLOT(logClientDisConnected(qint32)));

    QNetworkInterface inter;
//    QHostAddress myaddress = hostInfo.localHostName();
    QList<QHostAddress>myaddress = inter.allAddresses();
//    ui->seriptext->setText(hostInfo.localDomainName());

}

GUI::~GUI()
{
    delete ui;
}


void GUI::closeEvent(QCloseEvent *event) {
    qDebug() << "closed";
    if(soundThread->isRunning()) {
        connect(soundThread, SIGNAL(destroyed()), this, SLOT(close()));
        emit stopSoundWorker();
        event->ignore();
    }
    else {
        event->accept();
    }
}

void GUI::showClientList() {
    showClients->show();
}

void GUI::logClientConnected(ClientInfo *client) {
    ui->logDisplay->insertPlainText(generateTimeStamp() + QString(" " + client->getClientTypeStr() + " (id= %1) connected! \n").arg(client->getId()));
}

void GUI::logClientDisconnected(qint32 id) {
    ui->logDisplay->insertPlainText(QString("Client (id= %1) disconnected! \n").arg(id));
}

QString GUI::generateTimeStamp() {
    QDateTime time = QDateTime::currentDateTime();
    return time.toString();
}


