#include "gui.h"
#include "ui_gui.h"

GUI::GUI(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GUI)
{
    ui->setupUi(this);

    qRegisterMetaType<QHostAddress>("QHostAddress");

    init();
}

void GUI::init() {
    //create settings dialog
    //this class also holds valuable information, such userid, server address, communication ports, etc.
    settings = new Settings(this);

    //create servercommunicator class
    serverCommunicator = new ServerCommunicator(settings, this);

    //set up logindialog
    loginDialog = new LoginDialog(settings, this);
    //login request passed from logindialog to servercommunicator
    connect(loginDialog, SIGNAL(sendLoginRequest()), serverCommunicator, SLOT(sendLoginRequest()));
    //logout request passed from logindialog to servercommunicator
    connect(loginDialog, SIGNAL(sendLogoutRequest()), serverCommunicator, SLOT(logout()));
    //signal emitted when authentification was successfull
    connect(serverCommunicator, SIGNAL(authentificationSucces(qint32)), loginDialog, SLOT(authentificationSucces(qint32)));
    //signal emitted when authentification failed
    connect(serverCommunicator, SIGNAL(authentificationFailed()), loginDialog, SLOT(authentificationFailed()));
    //signal emitted when authentification timed out
    connect(serverCommunicator, SIGNAL(authentificationTimedOut()), loginDialog, SLOT(authentificationTimedOut()));

    //****GUI SIGNALS***
    connect(ui->startBroadcastBtn, SIGNAL(pressed()), this, SLOT(startBroadcast()));
    connect(ui->menuPreferences, SIGNAL(triggered(QAction*)), this, SLOT(menuTriggered(QAction*)));
    connect(ui->newChannelBtn, SIGNAL(clicked()), this, SLOT(startNewChannel()));
    connect(this, SIGNAL(sendLogoutRequest()), serverCommunicator, SLOT(logout()));
    //******

    broadcastDataSize = 0;
    broadcastDataPerSec = 0;
    cntBroadcastTime = 0;

    broadcastTimer.setInterval(1000);
    connect(&broadcastTimer, SIGNAL(timeout()), this, SLOT(updateBroadcastTime()));

    //server down
    connect(serverCommunicator, SIGNAL(serverDown()), this, SLOT(serverDownHandle()));

    isSpeakerRunning = false;

    newChannelDialog = NULL;

}

void GUI::login() {
    loginDialog->init();
    loginDialog->exec();
    if(!loginDialog->authentificationStatus()) {
        QTimer::singleShot(0, this, SLOT(close()));
    }
    else {
        if(!this->isVisible()) {
            this->show();
        }
        QHostAddress address = settings->getServerAddress();
        ui->ipaddressLabel->setText(address.toString());
        ui->portLabel->setText(QString::number(settings->getClientPort()));
        try {
            newChannelDialog = new NewChannelDialog(this);
            connect(newChannelDialog, SIGNAL(requestNewChannel(Datagram)), serverCommunicator, SLOT(sendDatagram(Datagram)));
            connect(serverCommunicator, SIGNAL(newChannelAckReceived(Datagram)), newChannelDialog, SLOT(newChannelAck(Datagram)));
            connect(newChannelDialog, SIGNAL(closeChannel(Datagram)), serverCommunicator, SLOT(sendDatagram(Datagram)));
            newChannelDialog->setClientId(settings->getClientId());
            newChannelDialog->setAudioDeviceInfo(settings->getInputDevice());
        }
        catch(InvalidIdException *ex) {
            qDebug() << ex->message();
            delete ex;
            this->close();
        }
        catch(NoAudioDeviceException *ex) {
            qDebug() << ex->message();
            newChannelDialog = NULL;
            delete ex;
        }
    }
}

void GUI::serverDownHandle() {
    emit stopBroadcast();
//    emit stopPlaybackSD();
    stopChannel();
    login();
    qDebug() << "serverDown login";
}

GUI::~GUI()
{
    delete ui;
    qDebug() << "GUI destruct!";
}

void GUI::startNewChannel() {
    //check if it is channel already connected
    if(!newChannelDialog->isChannelAvailable()) {
        newChannelDialog->exec();
        if(newChannelDialog->isChannelAvailable()) {
            const ChannelInfo* chInfo = newChannelDialog->getChannelInformation();
            ui->channelLangText->setText(chInfo->getLanguage());
            ui->sampleRateText->setText(QString::number(chInfo->getSampleRate()) + "bits");
            ui->sampleSizeText->setText(QString::number(chInfo->getSampleSize()) + "Hz");
            ui->channelNrText->setText(QString::number(chInfo->getChannels()));
            ui->codecText->setText(chInfo->getCodec());
            ui->newChannelBtn->setText("Close channel");
        }
    }
    else {
        //stop channel
        stopChannel();
    }

}

void GUI::stopChannel() {
    if(newChannelDialog != NULL) {
        newChannelDialog->sendCloseChannelReq();
    }
    ui->channelLangText->setText("-");
    ui->sampleRateText->setText("-");
    ui->sampleSizeText->setText("-");
    ui->channelNrText->setText("-");
    ui->codecText->setText("-");
    ui->newChannelBtn->setText("New channel");
    if(isSpeakerRunning) {
        startBroadcast();
    }
}

void GUI::setDataSent(int size) {
    broadcastDataSize += size;
    broadcastDataPerSec +=  size;
    ui->dataSent->setText(QString::number(broadcastDataSize / 1024) + "kByte");
}

void GUI::startBroadcast() {
    if(!isSpeakerRunning) {
        if(newChannelDialog->isChannelAvailable()) {
            QAudioFormat speakerFormat = newChannelDialog->getAudioFormat();

            try {
                QAudioDeviceInfo deviceInfo = settings->getInputDevice();
                QHostAddress serverAddress = settings->getServerAddress();
                qint32 port = settings->getClientPortForSound();
                qint32 id = settings->getClientId();

                //initialize speaker worker
                if(settings->testMode()) {
                    speakerWorker = new TestSpeaker;
                }
                else {
                    speakerWorker = new Speaker;
                }
                //initialize speaker thread
                speakerThread = new QThread;
                //thread lifetime management
                speakerWorker->moveToThread(speakerThread);
                connect(speakerWorker, SIGNAL(finished()), speakerThread, SLOT(quit()));
                connect(speakerWorker, SIGNAL(finished()), speakerWorker, SLOT(deleteLater()));
                connect(speakerThread, SIGNAL(finished()), speakerThread, SLOT(deleteLater()));
                connect(speakerWorker, SIGNAL(errorMessage(QString)), this, SLOT(errorMessage(QString)));

                //signal emited when user starts or stops the playback
                connect(this, SIGNAL(startBroadcast(QAudioFormat, QAudioDeviceInfo, QHostAddress, qint32, qint32)),
                        speakerWorker, SLOT(start(QAudioFormat, QAudioDeviceInfo, QHostAddress, qint32, qint32)));
                //recording state
                connect(speakerWorker, SIGNAL(recordingState(bool)), this, SLOT(changeBroadcastButtonState(bool)));
                //updating the ui with the traffic information
                connect(speakerWorker, SIGNAL(dataSent(int)), this, SLOT(setDataSent(int)));
                //stop broadcasting
                connect(this, SIGNAL(stopBroadcast()), speakerWorker, SLOT(stop()));
                //start the thread
                speakerThread->start();
                //start the broadcast
                emit startBroadcast(speakerFormat, deviceInfo, serverAddress, port, id);

            }
            catch(NoAudioDeviceException *ex) {
                errorMessage(ex->message());
                delete ex;
            }
        }
    }
    else {
        //stop the broadcast
        emit stopBroadcast();
    }
}

void GUI::changeBroadcastButtonState(bool isSpeakerRunning) {
    if(isSpeakerRunning) {
        ui->startBroadcastBtn->setText("Stop Broadcast");
        broadcastDataSize = 0;
        broadcastTimerStart();
        this->isSpeakerRunning = true;
    }
    else {
        ui->startBroadcastBtn->setText("Start Broadcast");
        broadcastDataSize = 0;
        broadcastTimerStop();
        this->isSpeakerRunning = false;

    }
}

void GUI::menuTriggered(QAction* action) {
    if(action == ui->actionSettings) {
        settings->exec();
    }
    else {
        return;
    }
}


void GUI::updateBroadcastTime() {
    cntBroadcastTime++;
    ui->timePassed->setText(QString::number(cntBroadcastTime) + " seconds");
    ui->speed->setText(QString::number(broadcastDataPerSec / 1024) + " KB/s");
    broadcastDataPerSec = 0;
}

void GUI::broadcastTimerStart() {
    cntBroadcastTime = 0;
    broadcastTimer.start();
}

void GUI::broadcastTimerStop() {
    broadcastTimer.stop();
}

void GUI::errorMessage(QString message) {
    QMessageBox msg;
    msg.setText(message);
    msg.setStandardButtons(QMessageBox::Ok);
    msg.exec();
    return;
}

void GUI::closeEvent(QCloseEvent *event) {
    if(isSpeakerRunning) {
        connect(speakerThread, SIGNAL(destroyed()), this, SLOT(close()));
        emit stopBroadcast();
        event->ignore();
    }
    else {
        event->accept();
    }
    emit sendLogoutRequest();
}


