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
    isPlaying = false;
    doubleClickEnabled = true;

    //init settings
    settings = settings = new QSettings(QSettings::IniFormat, QSettings::UserScope,
                                        Settings::organization_label, Settings::appname_label, this);

    //initialize timers
    timer.setInterval(1000);
    connect(&timer, SIGNAL(timeout()), this, SLOT(updateTime()));

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

    //initialize volume slider
    ui->volumeSlider->setMaximum(100);
    ui->volumeSlider->setValue(50);

    //****GUI SIGNALS***
    connect(ui->volumeSlider, SIGNAL(sliderMoved(int)), this, SLOT(volumeChangedSlot()));
    connect(ui->playButton, SIGNAL(clicked()), this, SLOT(playbackButtonPushed()));
    connect(ui->channelList, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(changeChannelOnDoubleClick(QModelIndex)));
    connect(ui->menuSettings, SIGNAL(triggered(QAction*)), this, SLOT(menuTriggered(QAction*)));
    connect(ui->newChannelButton, SIGNAL(clicked()), this, SLOT(addNewChannel()));
    connect(ui->recordButton, SIGNAL(clicked()), this, SLOT(startRecordPushed()));
    connect(ui->pauseRec, SIGNAL(clicked()), this, SLOT(pauseRecordPushed()));
    connect(ui->deleteChannelButton, SIGNAL(clicked()), this, SLOT(deleteChannel()));
    connect(this, SIGNAL(sendLogoutRequest()), serverCommunicator, SLOT(logout()));
    //******

    //model for channellist
    channelModel = new ChannelModel(this);
    ui->channelList->setModel(channelModel);
    connect(channelModel, SIGNAL(error(QString)), this, SLOT(showErrorMessage(QString)));
    connect(serverCommunicator, SIGNAL(serverList(QByteArray)), channelModel, SLOT(newChannelList(QByteArray)));
    connect(serverCommunicator, SIGNAL(channelConnected(ChannelInfo)), channelModel, SLOT(addNewChannel(ChannelInfo)));

    //add/remove channel from channellist(local channel, this will not start a new channel on server)
    try {
        addNewChannelMan = new AddNewChannelFromGui(getOutputDevice(), this);
        connect(addNewChannelMan, SIGNAL(newUserCreatedChannel(ChannelInfo)), channelModel, SLOT(addNewUserCreatedChannel(ChannelInfo)));
    }
    catch(NoAudioDeviceException *exc) {
        addNewChannelMan = NULL;
        qDebug() << exc->message();
        delete exc;
    }
    connect(serverCommunicator, SIGNAL(removeChannel(qint32)), channelModel, SLOT(deleteChannel(qint32)));
    connect(serverCommunicator, SIGNAL(removeChannel(qint32)), this, SLOT(channelStopedByServer(qint32)));

    //server down
    connect(serverCommunicator, SIGNAL(serverDown()), this, SLOT(serverDownHandle()));
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
        try {
            serverCommunicator->requestChannelList();
        }
        catch(InvalidIdException *ex) {
            qDebug() << ex->message();
            delete ex;
            this->close();
        }
    }
}

void GUI::serverDownHandle() {
    emit stopPlaybackSD();
    login();
    qDebug() << "serverDown login";
}

GUI::~GUI()
{
    delete ui;
}


void GUI::setDataReceived(int size) {
    dataSize += size;
    ui->dataReceived->setText(QString::number(dataSize / 1024) + "kByte");
    dataPerSec += size;
}

//this SLOT is called when the Start button is pushed
void GUI::playbackButtonPushed() {
    if(!isPlaying) {
        QModelIndex selectedIndex = ui->channelList->currentIndex();
        try {
            selectedChannel = channelModel->getData(selectedIndex);
            QAudioDeviceInfo device = getOutputDevice();
            QHostAddress address = serverCommunicator->getServerAddress();
            createListenerThread();
            emit startListening(selectedChannel, device, address, 0.5);
        }
        catch(ChannelListException *ex) {
            showErrorMessage(ex->message());
            delete ex;
            return;
        }
        catch(NoAudioDeviceException *ex) {
            showErrorMessage(ex->message());
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
    connect(listenerWorker, SIGNAL(changePlayButtonState(bool)), this, SLOT(changePlayButtonState(bool)), Qt::BlockingQueuedConnection);
    //volume
    connect(this, SIGNAL(volumeChanged(qreal)), listenerWorker, SLOT(volumeChanged(qreal)));
    //record sound
    connect(this, SIGNAL(startRecord(QString, QString)), listenerWorker, SLOT(startRecord(QString, QString)));
    connect(this, SIGNAL(pauseRecord()), listenerWorker, SLOT(pauseRecord()));
    connect(listenerWorker, SIGNAL(dataReceived(int)), this, SLOT(setDataReceived(int)));
    connect(listenerWorker, SIGNAL(changeRecordButtonState(RecordAudio::STATE)), this, SLOT(changeRecordButtonState(RecordAudio::STATE)));
    connect(listenerWorker, SIGNAL(changePauseButtonState(RecordAudio::STATE)), this, SLOT(changePauseButtonState(RecordAudio::STATE)));
    //error message
    connect(listenerWorker, SIGNAL(errorMessage(QString)), this, SLOT(showErrorMessage(QString)));
}

void GUI::changePlayButtonState(bool isPlaying) {
    this->isPlaying = isPlaying;
    if(isPlaying) {
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


int GUI::getVolume() {
    return ui->volumeSlider->value();
}

void GUI::volumeChangedSlot() {
    qreal volume = ui->volumeSlider->value() / 100.0;
    ui->volumeLabel->setText(QString::number(ui->volumeSlider->value()) + QString("%"));
    emit volumeChanged(volume);
}

void GUI::changeChannelOnDoubleClick(QModelIndex index) {
    qDebug() << index.row();
    if(doubleClickEnabled) {
        if(isPlaying) {
            doubleClickEnabled = false;
            connect(listenerThread, SIGNAL(destroyed()), this, SLOT(startNewChannelOnDistroy()));
        }
        playbackButtonPushed();
    }
}

void GUI::startNewChannelOnDistroy() {
    if(!isPlaying) {
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
        showErrorMessage("No input device found!");
    }
}

void GUI::menuTriggered(QAction* action) {
    if(action == ui->actionPreferences) {
        Settings *settingsDialog = new Settings;
        settingsDialog->setAttribute(Qt::WA_DeleteOnClose);
        settingsDialog->exec();
    }
    else {
        //exit
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
        ui->pauseRec->setText("Unpause");
        break;
    }
    case RecordAudio::RECORDING : {
        ui->pauseRec->setText("Pause");
    }
    }
}

//show any error message
void GUI::showErrorMessage(QString message) {
    QMessageBox msgBox;
    msgBox.setText(message);
    msgBox.exec();
}

void GUI::closeEvent(QCloseEvent *event) {
    if(isPlaying) {
        connect(listenerThread, SIGNAL(destroyed()), this, SLOT(close()));
        emit stopListening();
        event->ignore();
    }
    else {
        emit sendLogoutRequest();
        event->accept();
    }
}

//delete selected channel
void GUI::deleteChannel() {
    QModelIndex selectedIndex = ui->channelList->currentIndex();
    channelModel->deleteUserCreatedChannel(selectedIndex);
}

QAudioDeviceInfo GUI::getOutputDevice() const {
    QString audioDeviceName = settings->value(Settings::output_device_label, "").toString();
    QList<QAudioDeviceInfo> outputDevices = QAudioDeviceInfo::availableDevices(QAudio::AudioOutput);
    if(outputDevices.size() == 0) {
        NoAudioDeviceException *exception = new NoAudioDeviceException;
        exception->setMessage("No output aoudio device found!");
        throw exception;
//        return;
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

void GUI::channelStopedByServer(qint32 id) {
    if(isPlaying) {
        if(selectedChannel->getOwner() == id) {
            emit stopListening();
            showErrorMessage("Channel stoped by the server!");
        }
    }
}


