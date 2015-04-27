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

    //get settings
    settings = new Settings(this);

    //initialize timers
    timer.setInterval(1000);
    connect(&timer, SIGNAL(timeout()), this, SLOT(updateTime()));

    //initialize statusbar
    ui->statusBar->showMessage("Player stopped!");

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
        addNewChannelMan = new AddNewChannelFromGui(settings->getOutputDevice(), this);
        connect(addNewChannelMan, SIGNAL(newUserCreatedChannel(ChannelInfo)), channelModel, SLOT(addNewUserCreatedChannel(ChannelInfo)));
    }
    catch(NoAudioDeviceException *exc) {
        addNewChannelMan = NULL;
        qDebug() << exc->message();
        delete exc;
    }
    connect(serverCommunicator, SIGNAL(removeChannel(qint32)), channelModel, SLOT(deleteChannel(qint32)));

    //signals emitted when server is down
//    connect(this, SIGNAL(stopPslaybackSD()), listenerWorker, SLOT(stopPlayback()));

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
            QSharedPointer<ChannelInfo> selectedChannel = channelModel->getData(selectedIndex);
            QAudioDeviceInfo device = settings->getOutputDevice();
            QHostAddress address = settings->getServerAddress();
            createListenerThread();
            emit startListening(selectedChannel, device, address, 0.5);
        }
        catch(ChannelListException *ex) {
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
    connect(this, SIGNAL(startListening(QSharedPointer<ChannelInfo>, QAudioDeviceInfo, QHostAddress, int)),
            listenerWorker, SLOT(start(QSharedPointer<ChannelInfo>, QAudioDeviceInfo, QHostAddress, int)));
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
    connect(listenerWorker, SIGNAL(errorMessage(QString)), this, SLOT(showErrorMessage(QString)));
}

void GUI::changePlayButtonState(bool isPlaying) {
    this->isPlaying = isPlaying;
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


int GUI::getVolume() {
    return ui->volumeSlider->value();
}

void GUI::volumeChangedSlot() {
    emit volumeChanged(ui->volumeSlider->value() / 100.0);
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
        settings->exec();
    }
    else {
        //exit
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


