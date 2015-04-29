#include "gui.h"
#include "ui_gui.h"

GUI::GUI(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GUI)
{
    ui->setupUi(this);
    initialize();

    qRegisterMetaType< QSharedPointer<ChannelInfo> >("QSharedPointer<ChannelInfo>");
    qRegisterMetaType<QHostAddress>("QHostAddress");
    qRegisterMetaType<Settings::CODEC>("Settings::CODEC");
}

void GUI::initialize() {

    dataSize = 0;
    dataPerSec = 0;

    broadcastDataSize = 0;
    broadcastDataPerSec = 0;

    broadcasting_port = -1;

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


    //initialize volume slider
    ui->volumeSlider->setMaximum(100);
    ui->volumeSlider->setValue(50);

    //****GUI SIGNALS***
    connect(ui->volumeSlider, SIGNAL(sliderMoved(int)), this, SLOT(volumeChangedSlot()));
    connect(ui->playButton, SIGNAL(clicked()), this, SLOT(playbackButtonPushed()));
    connect(ui->newLocalChannel, SIGNAL(clicked()), this, SLOT(addNewChannel()));
    connect(ui->recordButton, SIGNAL(clicked()), this, SLOT(startRecordPushed()));
    connect(ui->pauseRec, SIGNAL(clicked()), this, SLOT(pauseRecordPushed()));
    connect(ui->startBroadcastBtn, SIGNAL(clicked()), this, SLOT(startBroadcast()));
    connect(ui->menuPreferences, SIGNAL(triggered(QAction*)), this, SLOT(menuTriggered(QAction*)));
    connect(ui->newChannelBtn, SIGNAL(clicked()), this, SLOT(startNewChannel()));
    connect(ui->deleteLocalChannel, SIGNAL(clicked()), this, SLOT(deleteChannel()));
    connect(ui->refreshButton, SIGNAL(clicked()), serverCommunicator, SLOT(requestChannelList()));
    connect(this, SIGNAL(sendLogoutRequest()), serverCommunicator, SLOT(logout()));
    connect(ui->channelList, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(changeChannelOnDoubleClick(QModelIndex)));

    //model for channellist
    channelModel = new ChannelModel(this);
    ui->channelList->setModel(channelModel);
    connect(channelModel, SIGNAL(error(QString)), this, SLOT(errorMessage(QString)));
    connect(serverCommunicator, SIGNAL(serverList(QByteArray)), channelModel, SLOT(newChannelList(QByteArray)));
    connect(serverCommunicator, SIGNAL(channelConnected(ChannelInfo)), channelModel, SLOT(addNewChannel(ChannelInfo)));

     //add/remove channel from channellist(local channel, this will not start a new channel on server)
    addNewChannelMan = new AddNewChannelFromGui(settings->getOutputDevice(), this);
    connect(addNewChannelMan, SIGNAL(newUserCreatedChannel(ChannelInfo)), channelModel, SLOT(addNewUserCreatedChannel(ChannelInfo)));
    connect(serverCommunicator, SIGNAL(removeChannel(qint32)), channelModel, SLOT(deleteChannel(qint32)));

    //initialize timers
    timer.setInterval(1000);
    connect(&timer, SIGNAL(timeout()), this, SLOT(updateTime()));

    broadcastTimer.setInterval(1000);
    connect(&broadcastTimer, SIGNAL(timeout()), this, SLOT(updateBroadcastTime()));

    newChannelDialog = new NewChannelDialog(this);
    connect(newChannelDialog, SIGNAL(requestNewChannel(Datagram)), serverCommunicator, SLOT(sendDatagram(Datagram)));
    connect(serverCommunicator, SIGNAL(newChannelAckReceived(Datagram)), newChannelDialog, SLOT(newChannelAck(Datagram)));
    connect(newChannelDialog, SIGNAL(closeChannel(Datagram)), serverCommunicator, SLOT(sendDatagram(Datagram)));

    //server down
    connect(serverCommunicator, SIGNAL(serverDown()), this, SLOT(serverDownHandle()));

    isListenerRunning = false;
    isSpeakerRunning = false;
    doubleClickEnabled = true;

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

            serverCommunicator->requestChannelList();
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
    stopChannel();
    login();
    qDebug() << "serverDown login";
}

GUI::~GUI()
{
    delete ui;
    qDebug() << "GUI destructor!";
}

void GUI::startNewChannel() {
    //check if it is channel already connected
    if(!newChannelDialog->isChannelAvailable()) {
        newChannelDialog->exec();
        if(newChannelDialog->isChannelAvailable()) {
            const ChannelInfo* chInfo = newChannelDialog->getChannelInformation();
            ui->channelLangText->setText(chInfo->getLanguage());
            ui->sampleRateText->setText(QString::number(chInfo->getSampleRate()));
            ui->sampleSizeText->setText(QString::number(chInfo->getSampleSize()));
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

//update GUI with sent data size
void GUI::setDataReceived(int size) {
    dataSize += size;
    ui->dataReceived->setText(QString::number(dataSize / 1024) + "kByte");
    dataPerSec += size;
}

//update GUI with received data size
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

//this SLOT is called when the Start button is pushed
void GUI::playbackButtonPushed() {
    if(!isListenerRunning) {
        QModelIndex selectedIndex = ui->channelList->currentIndex();
        try {
            QSharedPointer<ChannelInfo> selectedChannel = channelModel->getData(selectedIndex);
            QAudioDeviceInfo device = settings->getOutputDevice();
            QHostAddress address = settings->getServerAddress();
            createListenerThread();
            emit startListening(selectedChannel, device, address, 0.5);
        }
        catch(ChannelListException *ex) {
            errorMessage(ex->message());
            delete ex;
            return;
        }
    }
    else {
        emit stopListening();
    }
    ui->playButton->setEnabled(false);
}

void GUI::createListenerThread() {
    //initialize listenerWorker
    listenerWorker = new Listener;
    listenerThread = new QThread;
    listenerWorker->moveToThread(listenerThread);
    connect(listenerWorker, SIGNAL(finished()), listenerThread, SLOT(quit()));
    connect(listenerThread, SIGNAL(finished()), listenerThread, SLOT(deleteLater()));
    connect(listenerWorker, SIGNAL(finished()), listenerWorker, SLOT(deleteLater()));
    listenerThread->start();
    //playback state
    connect(this, SIGNAL(startListening(QSharedPointer<ChannelInfo>, QAudioDeviceInfo, QHostAddress, qreal)),
            listenerWorker, SLOT(start(QSharedPointer<ChannelInfo>, QAudioDeviceInfo, QHostAddress, qreal)));
    connect(this, SIGNAL(stopListening()), listenerWorker, SLOT(stop()));
    connect(listenerWorker, SIGNAL(changePlayButtonState(bool)), this, SLOT(changePlayButtonState(bool)));
    //volume
    connect(this, SIGNAL(volumeChanged(qreal)), listenerWorker, SLOT(volumeChanged(qreal)));
    //record sound
    connect(this, SIGNAL(startRecord(Settings::CODEC, QString)), listenerWorker, SLOT(startRecord(Settings::CODEC, QString)));
    connect(this, SIGNAL(pauseRecord()), listenerWorker, SLOT(pauseRecord()));
    connect(listenerWorker, SIGNAL(dataReceived(int)), this, SLOT(setDataReceived(int)));
    connect(listenerWorker, SIGNAL(changeRecordButtonState(RecordAudio::STATE)), this, SLOT(changeRecordButtonState(RecordAudio::STATE)));
    connect(listenerWorker, SIGNAL(changePauseButtonState(RecordAudio::STATE)), this, SLOT(changePauseButtonState(RecordAudio::STATE)));
    //error message
    connect(listenerWorker, SIGNAL(errorMessage(QString)), this, SLOT(errorMessage(QString)));
}

void GUI::changePlayButtonState(bool isPlaying) {
    this->isListenerRunning = isPlaying;
    if(isPlaying) {
        ui->playButton->setText("Stop");
        ui->playButton->setEnabled(true);
        receiverTimerStart();
    }
    else {
        ui->playButton->setText("Play");
        ui->playButton->setEnabled(true);
        receiverTimerStop();
    }
}


void GUI::receiverTimerStart() {
    cntTime = 0;
    timer.start();
}

void GUI::receiverTimerStop() {
    timer.stop();
}

void GUI::updateTime() {
    cntTime++;
    ui->timePassed->setText(QString::number(cntTime) + " secs");
    ui->speed->setText(QString::number(dataPerSec / 1024) + " KB/s");
    dataPerSec = 0;
}

void GUI::updateBroadcastTime() {
    cntBroadcastTime++;
    ui->timePassed_2->setText(QString::number(cntBroadcastTime) + " secs");
    ui->speed_2->setText(QString::number(broadcastDataPerSec / 1024) + " KB/s");
    broadcastDataPerSec = 0;
}

void GUI::broadcastTimerStart() {
    cntBroadcastTime = 0;
    broadcastTimer.start();
}

void GUI::broadcastTimerStop() {
    broadcastTimer.stop();
}

int GUI::getVolume() {
    return ui->volumeSlider->value();
}

//when volume is changed, emit signal to listener
void GUI::volumeChangedSlot() {
    emit volumeChanged(ui->volumeSlider->value());
}


void GUI::changeChannelOnDoubleClick(QModelIndex index) {
    qDebug() << index.row();
    if(doubleClickEnabled) {
        if(isListenerRunning) {
            doubleClickEnabled = false;
            connect(listenerThread, SIGNAL(destroyed()), this, SLOT(startNewChannelOnDistroy()));
        }
        playbackButtonPushed();
    }
}

void GUI::startNewChannelOnDistroy() {
    if(!isListenerRunning) {
        disconnect(listenerThread, SIGNAL(destroyed()), this, SLOT(startNewChannelOnDistroy()));
        playbackButtonPushed();
        doubleClickEnabled = true;
    }
}

void GUI::addNewChannel() {
    if(addNewChannelMan != NULL) {
        addNewChannelMan->show();
    }
    else {
        errorMessage("No input device found!");
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

void GUI::startRecordPushed() {
    Settings::CODEC codec = settings->getRecordCodec();
    QString path = settings->getRecordPath();
    emit startRecord(codec, path);
}

void GUI::pauseRecordPushed() {
    emit pauseRecord();
}

void GUI::changeRecordButtonState(RecordAudio::STATE state) {
    switch(state) {
    case RecordAudio::STOPPED : {
        ui->recordButton->setText("Record");
        ui->pauseRec->setText("Pause");
        break;
    }
    case RecordAudio::RECORDING : {
        ui->recordButton->setText("Stop Record");
    }
    }
}

void GUI::changePauseButtonState(RecordAudio::STATE state) {
    switch(state) {
    case RecordAudio::PAUSED : {
        ui->pauseRec->setText("Unpause");
        break;
    }
    case RecordAudio::RECORDING : {
        ui->pauseRec->setText("Pause");
    }
    }
}

//show any error message
void GUI::errorMessage(QString message) {
    QMessageBox msgBox;
    msgBox.setText(message);
    msgBox.exec();
}

void GUI::closeEvent(QCloseEvent *event) {
    if(isSpeakerRunning) {
        connect(speakerThread, SIGNAL(destroyed()), this, SLOT(close()));
        emit stopBroadcast();
        event->ignore();
    }
    else {
        if(isListenerRunning) {
            connect(listenerThread, SIGNAL(destroyed()), this, SLOT(close()));
            emit stopListening();
            event->ignore();
        }
        else {
            emit sendLogoutRequest();
            event->accept();
        }
    }
}

//delete selected channel
void GUI::deleteChannel() {
    QModelIndex selectedIndex = ui->channelList->currentIndex();
    channelModel->deleteUserCreatedChannel(selectedIndex);
}


