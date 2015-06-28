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

    //init settings
    settings = settings = new QSettings(QSettings::IniFormat, QSettings::UserScope,
                                        Settings::organization_label, Settings::appname_label, this);

    //create servercommunicator class
    serverCommunicator = new ServerCommunicator(this);

    //set up logindialog
    loginDialog = new LoginDialog(this);
    //login request passed from logindialog to servercommunicator
    connect(loginDialog, SIGNAL(sendLoginRequest(QString)), serverCommunicator, SLOT(sendLoginRequest(QString)));
    //logout request passed from logindialog to servercommunicator
    connect(loginDialog, SIGNAL(sendLogoutRequest()), serverCommunicator, SLOT(logout()));
    //signal emitted when authentification was successfull
    connect(serverCommunicator, SIGNAL(authentificationSucces(qint32)), loginDialog, SLOT(authentificationSucces(qint32)));
    //signal emitted when authentification failed
    connect(serverCommunicator, SIGNAL(authentificationFailed()), loginDialog, SLOT(authentificationFailed()));
    //signal emitted when authentification timed out
    connect(serverCommunicator, SIGNAL(authentificationTimedOut()), loginDialog, SLOT(authentificationTimedOut()));

    connect(this, SIGNAL(startPacketLog(QMutex*,QFile*)), serverCommunicator, SLOT(startPacketLog(QMutex*,QFile*)));
    connect(this, SIGNAL(stopPacketLog()), serverCommunicator, SLOT(stopPacketLog()));


    //initialize volume slider
    ui->volumeSlider->setMaximum(100);
    ui->volumeSlider->setValue(50);

    //****GUI SIGNALS***
    connect(ui->volumeSlider, SIGNAL(sliderMoved(int)), this, SLOT(volumeChangedSlot()));
    connect(ui->playButton, SIGNAL(clicked()), this, SLOT(playbackButtonPushed()));
    connect(ui->newLocalChannel, SIGNAL(clicked()), this, SLOT(addNewLocalChannel()));
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

    connect(serverCommunicator, SIGNAL(removeChannel(qint32)), channelModel, SLOT(deleteChannel(qint32)));
    connect(serverCommunicator, SIGNAL(removeChannel(qint32)), this, SLOT(channelStopedByServer(qint32)));

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
    isLogging = false;
    logMutex = new QMutex;
    if(settings->value(Settings::log_label, false).toBool()) {
        preparePacketLog(settings->value(Settings::logPath_label, QDir::currentPath()).toString());
    }

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
        QHostAddress address = serverCommunicator->getServerAddress();
        ui->ipaddressLabel->setText(address.toString());
        ui->portLabel->setText(QString::number(serverCommunicator->getClientPort()));
        try {
            newChannelDialog = new NewChannelDialog(this);
            connect(newChannelDialog, SIGNAL(requestNewChannel(Datagram)), serverCommunicator, SLOT(sendDatagram(Datagram)));
            connect(serverCommunicator, SIGNAL(newChannelAckReceived(Datagram)), newChannelDialog, SLOT(newChannelAck(Datagram)));
            connect(newChannelDialog, SIGNAL(closeChannel(Datagram)), serverCommunicator, SLOT(sendDatagram(Datagram)));
            newChannelDialog->setClientId(serverCommunicator->getClientId());
            newChannelDialog->setAudioDeviceInfo(getInputDevice());

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
            ui->sampleRateText->setText(QString::number(chInfo->getSampleRate()) + QString(" Hz"));
            ui->sampleSizeText->setText(QString::number(chInfo->getSampleSize()) + QString(" bits"));
            if(chInfo->getChannels() == 1) {
                ui->channelNrText->setText(QString("mono"));
            }
            else {
                if(chInfo->getChannels() == 2) {
                    ui->channelNrText->setText(QString("stereo"));
                }
                else {
                    ui->channelNrText->setText(QString::number(chInfo->getChannels()));
                }
            }
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
                QAudioDeviceInfo deviceInfo = getInputDevice();
                QHostAddress serverAddress = serverCommunicator->getServerAddress();
                qint32 port = serverCommunicator->getClientPortForSound();
                qint32 id = serverCommunicator->getClientId();

                bool testMode = settings->value(Settings::testmode_label, false).toBool();

                //initialize speaker worker
                if(testMode) {
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
                connect(this, SIGNAL(startPacketLog(QMutex*, QFile*)), speakerWorker, SLOT(startPacketLog(QMutex*, QFile*)));
                connect(this, SIGNAL(stopPacketLog()), speakerWorker, SLOT(stopPacketLog()));
                if(isLogging) {
                    emit startPacketLog(logMutex, packetLoggerFile);
                }
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
            selectedChannel = channelModel->getData(selectedIndex);
            QAudioDeviceInfo device = getOutputDevice();
            QHostAddress address = serverCommunicator->getServerAddress();
            createListenerThread();
            emit startListening(selectedChannel, device, address, 0.5);
        }
        catch(ChannelListException *ex) {
            errorMessage(ex->message());
            delete ex;
            return;
        }
        catch(NoAudioDeviceException *ex) {
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
    connect(this, SIGNAL(startRecord(QString, QString)), listenerWorker, SLOT(startRecord(QString, QString)));
    connect(this, SIGNAL(pauseRecord()), listenerWorker, SLOT(pauseRecord()));
    connect(listenerWorker, SIGNAL(dataReceived(int)), this, SLOT(setDataReceived(int)));
    connect(listenerWorker, SIGNAL(changeRecordButtonState(RecordAudio::STATE)), this, SLOT(changeRecordButtonState(RecordAudio::STATE)));
    connect(listenerWorker, SIGNAL(changePauseButtonState(RecordAudio::STATE)), this, SLOT(changePauseButtonState(RecordAudio::STATE)));
    //error message
    connect(listenerWorker, SIGNAL(errorMessage(QString)), this, SLOT(errorMessage(QString)));
    //packetlog
    connect(this, SIGNAL(startPacketLog(QMutex*, QFile*)), listenerWorker, SLOT(startPacketLog(QMutex*, QFile*)));
    connect(this, SIGNAL(stopPacketLog()), listenerWorker, SLOT(stopPacketLog()));
    if(isLogging) {
        emit startPacketLog(logMutex, packetLoggerFile);
    }
}

void GUI::changePlayButtonState(bool isPlaying) {
    isListenerRunning = isPlaying;
    if(isListenerRunning) {
        ui->playButton->setText("Stop");
        ui->playButton->setEnabled(true);
        receiverTimerStart();

        ui->channelPlaying->setText(selectedChannel->getLanguage());
        ui->portPlaying->setText(QString::number(selectedChannel->getOutPort()));
        ui->sampleRatePlaying->setText(QString::number(selectedChannel->getSampleRate()) + " Hz");
        ui->sampleSizePlaying->setText(QString::number(selectedChannel->getSampleSize()) + " bits");

    }
    else {
        ui->playButton->setText("Play");
        ui->playButton->setEnabled(true);
        receiverTimerStop();

        ui->channelPlaying->setText("-");
        ui->portPlaying->setText("-");
        ui->sampleRatePlaying->setText("-");
        ui->sampleSizePlaying->setText("-");
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
    qreal volume = ui->volumeSlider->value() / 100.0;
    ui->volumeLabel->setText(QString::number(ui->volumeSlider->value()) + QString("%"));
    emit volumeChanged(volume);
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

void GUI::addNewLocalChannel() {
    //add/remove channel from channellist(local channel, this will not start a new channel on server)
    try {
        AddNewChannelFromGui *addNewChannelFromGUI = new AddNewChannelFromGui(getOutputDevice(), this);
        connect(addNewChannelFromGUI, SIGNAL(newUserCreatedChannel(ChannelInfo)), channelModel, SLOT(addNewUserCreatedChannel(ChannelInfo)));
        addNewChannelFromGUI->show();
    }
    catch(NoAudioDeviceException *exc) {
        errorMessage(exc->message());
        delete exc;
    }
}

void GUI::menuTriggered(QAction* action) {
    if(action == ui->actionSettings) {
        Settings *settingsDialog = new Settings;
        settingsDialog->setAttribute(Qt::WA_DeleteOnClose);
        connect(settingsDialog, SIGNAL(packetLogStarted(QString)), this, SLOT(preparePacketLog(QString)));
        connect(settingsDialog, SIGNAL(packetLogStopped()), this, SLOT(finalizePacketLog()));
        settingsDialog->show();
        return;
    }
    if(action == ui->actionExit) {
        this->close();
    }
}

void GUI::startRecordPushed() {
    QString codec = settings->value(Settings::record_codec_label, "wav").toString();
    QString path = settings->value(Settings::record_path_label, QDir::currentPath()).toString();
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
        ui->pauseRec->setText("Resume");
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

void GUI::preparePacketLog(QString path) {
    if(!isLogging) {
        QString fileName = "/translatorLog.txt";
        packetLoggerFile = new QFile(path + fileName, this);
        if(!packetLoggerFile->open(QIODevice::WriteOnly)) {
            emit errorMessage("translatorLog.txt could not been created!");
            return;
        }
        emit startPacketLog(logMutex, packetLoggerFile);
        isLogging = true;
    }
}

void GUI::finalizePacketLog() {
    if(isLogging) {
        emit stopPacketLog();
        packetLoggerFile->close();
        delete packetLoggerFile;
        isLogging = false;
    }
}

//get outputdevice from QSettings
//if it's not set a device, the default device will be returned
//it there is no outputdevice, exception is thrown
QAudioDeviceInfo GUI::getOutputDevice() const {
    QString audioDeviceName = settings->value(Settings::output_device_label, "").toString();
    QList<QAudioDeviceInfo> outputDevices = QAudioDeviceInfo::availableDevices(QAudio::AudioOutput);
    if(outputDevices.size() == 0) {
        NoAudioDeviceException *exception = new NoAudioDeviceException;
        exception->setMessage("No output aoudio device found!");
        throw exception;
    }
    else {
        foreach (const QAudioDeviceInfo it, outputDevices) {
            if(it.deviceName().compare(audioDeviceName) == 0) {
                return it;
            }
        }
        return QAudioDeviceInfo::defaultOutputDevice();
    }
}

//get inputdevice from QSettings
//if it's not set a device, the default device will be returned
//it there is no inputdevice, exception is thrown
QAudioDeviceInfo GUI::getInputDevice() const {
    QString audioDeviceName = settings->value(Settings::input_device_label, "").toString();
    QList<QAudioDeviceInfo> inputDevices = QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
    if(inputDevices.size() == 0) {
        NoAudioDeviceException *exception = new NoAudioDeviceException;
        exception->setMessage("No input aoudio device found!");
        throw exception;
    }
    else {
        foreach (const QAudioDeviceInfo it, inputDevices) {
            if(it.deviceName().compare(audioDeviceName) == 0) {
                return it;
            }
        }
        return QAudioDeviceInfo::defaultInputDevice();
    }
}

void GUI::channelStopedByServer(qint32 id) {
    if(isListenerRunning) {
        if(selectedChannel->getOwner() == id) {
            emit stopListening();
            errorMessage("Channel stoped by the server!");
        }
    }
}


