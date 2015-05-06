#include "gui.h"
#include "ui_gui.h"

GUI::GUI(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GUI)
{
    ui->setupUi(this);

    init();
}

GUI::~GUI()
{
    delete ui;
}

void GUI::init() {
    channelModel = new ChannelModel(this);
    clientModel = new ClientModel(this);

    mutex = new QMutex;

    settings = new QSettings(QSettings::IniFormat, QSettings::UserScope, Settings::organization_label, Settings::appName_label, this);
    QString addressStr = settings->value(Settings::bcast_label, "192.168.0.255").toString();
    QHostAddress address(addressStr);
    soundWorker = new AcceptData(address, mutex);
    soundThread = new QThread;
    soundWorker->moveToThread(soundThread);
    connect(soundThread, SIGNAL(started()), soundWorker, SLOT(init()));
    connect(soundWorker, SIGNAL(finished()), soundThread, SLOT(quit()));
    connect(this, SIGNAL(stopSoundWorker()), soundWorker, SLOT(stopWorker()), Qt::DirectConnection);
    connect(soundThread, SIGNAL(finished()), soundThread, SLOT(deleteLater()));
    connect(soundWorker, SIGNAL(finished()), soundWorker, SLOT(deleteLater()));
    soundThread->start();

    //menu handlers
    connect(ui->actionConnected_Clients, SIGNAL(triggered()), this, SLOT(showClientList()));
    connect(ui->actionSettings, SIGNAL(triggered()), this, SLOT(showSettings()));
    connect(ui->actionStop_server_and_Exit, SIGNAL(triggered()), this, SLOT(close()));

    ui->channelTab->setModel(channelModel);
    manageClients = new ManageClients(clientModel, channelModel, mutex);
    connect(manageClients, SIGNAL(newChannelAdded(ChannelInfo)), channelModel, SLOT(addNewChannel(ChannelInfo)));
    connect(manageClients, SIGNAL(newChannelAdded(ChannelInfo)), soundWorker, SLOT(addChannel(ChannelInfo)));
    connect(manageClients, SIGNAL(channelClosed(qint32)), channelModel, SLOT(deleteChannel(qint32)));
    connect(manageClients, SIGNAL(channelClosed(qint32)), soundWorker, SLOT(removeChannel(qint32)));

    showClients = new ShowClients(clientModel, this);
    connect(manageClients, SIGNAL(newClientConnected(ClientInfo*)), clientModel, SLOT(addClient(ClientInfo*)));
    connect(manageClients, SIGNAL(newClientConnected(ClientInfo*)), this, SLOT(logClientConnected(ClientInfo*)));

    connect(manageClients, SIGNAL(clientConnectionAck(qint32)), clientModel, SLOT(setAck(qint32)));
    connect(manageClients, SIGNAL(clientDisconnectedSignal(qint32)), clientModel, SLOT(removeClient(qint32)));
    connect(manageClients, SIGNAL(clientDisconnectedSignal(qint32)), this, SLOT(logClientDisconnected(qint32)));

    connect(clientModel, SIGNAL(clientTimedOut(qint32)), this, SLOT(logClientTimedOut(qint32)));
    connect(clientModel, SIGNAL(clientTimedOut(qint32)), channelModel, SLOT(deleteChannel(qint32)));
    connect(clientModel, SIGNAL(clientTimedOut(qint32)), soundWorker, SLOT(removeChannel(qint32)));

    foreach( QNetworkInterface interface, QNetworkInterface::allInterfaces() ){
        if( interface.humanReadableName().contains( "Local Area" ) ) {
            foreach (const QHostAddress &address, interface.allAddresses()) {
                if (address.protocol() == QAbstractSocket::IPv4Protocol && address != QHostAddress(QHostAddress::LocalHost)) {
                     if(address.toString().startsWith("192")) {
                         ui->seriptext->setText(address.toString());
                         break;
                     }
                }
            }
            break;
        }
    }

    connect(this, SIGNAL(startPacketLog(QFile*)), manageClients, SLOT(startPacketLog(QFile*)));
    connect(this, SIGNAL(startPacketLog(QFile*)), soundWorker, SLOT(startPacketLog(QFile*)));
    connect(this, SIGNAL(stopPacketLog()), manageClients, SLOT(stopPacketLog()));
    connect(this, SIGNAL(stopPacketLog()), soundWorker, SLOT(stopPacketLog()));

    isLogging = false;
    settings = new QSettings(QSettings::IniFormat, QSettings::UserScope, Settings::organization_label, Settings::appName_label, this);
    if(settings->value(Settings::log_label, false).toBool()) {
        preparePacketLog(settings->value(Settings::logPath_label, QDir::currentPath()).toString());
    }
}


void GUI::closeEvent(QCloseEvent *event) {
    qDebug() << "closed";
    this->hide();
    if(soundThread->isRunning()) {
        connect(soundThread, SIGNAL(destroyed()), this, SLOT(close()));
        emit stopSoundWorker();
        soundThread->quit();
        event->ignore();
    }
    else {
        manageClients->serverDown();
        event->accept();
    }
}

void GUI::showClientList() {
    showClients->show();
}

void GUI::showSettings() {
    Settings *settingsDialog = new Settings;
    settingsDialog->setAttribute(Qt::WA_DeleteOnClose);
    connect(settingsDialog, SIGNAL(broadcastAddressChanged(QString)), soundWorker, SLOT(changeBroadcastAddress(QString)));
    connect(settingsDialog, SIGNAL(packetLogStarted(QString)), this, SLOT(preparePacketLog(QString)));
    connect(settingsDialog, SIGNAL(packetLogStopped()), this, SLOT(finalizePacketLog()));
    settingsDialog->show();
}

void GUI::preparePacketLog(QString path) {
    if(!isLogging) {
        QString fileName = "/serverLog.txt";
        packetLoggerFile = new QFile(path + fileName, this);
        if(!packetLoggerFile->open(QIODevice::WriteOnly)) {
            emit errorMessage("serverLog.txt could not been created!");
            return;
        }
        emit startPacketLog(packetLoggerFile);
        isLogging = true;
    }
}

void GUI::finalizePacketLog() {
    if(isLogging) {
        emit stopPacketLog();
        packetLoggerFile->close();
        delete packetLoggerFile;
    }
}

void GUI::logClientConnected(ClientInfo *client) {
    ui->logDisplay->insertPlainText(generateTimeStamp() + QString(" " + client->getClientTypeStr() + " (id= %1) connected! \n").arg(client->getId()));
}

void GUI::logClientDisconnected(qint32 id) {
    ui->logDisplay->insertPlainText(generateTimeStamp() + QString(" Client (id= %1) disconnected! \n").arg(id));
}

void GUI::logClientTimedOut(qint32 id) {
    ui->logDisplay->insertPlainText(generateTimeStamp() + QString(" Client (id= %1) timed out! \n").arg(id));
}

QString GUI::generateTimeStamp() {
    QDateTime time = QDateTime::currentDateTime();
    return time.toString();
}


