#include "gui.h"
#include "ui_gui.h"

GUI::GUI(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GUI)
{
    ui->setupUi(this);
    initialize();

    qRegisterMetaType< QSharedPointer<ChannelInfo> >("QSharedPointer<ChannelInfo>");
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
    connect(ui->newChannelButton, SIGNAL(clicked()), this, SLOT(addNewChannel()));
    connect(ui->recordButton, SIGNAL(clicked()), this, SLOT(startRecordPushed()));
    connect(ui->pauseRec, SIGNAL(clicked()), this, SLOT(pauseRecordPushed()));
    connect(ui->startBroadcastBtn, SIGNAL(clicked()), this, SLOT(startBroadcast()));
    connect(ui->menuPreferences, SIGNAL(triggered(QAction*)), this, SLOT(menuTriggered(QAction*)));
    connect(ui->newChannelBtn, SIGNAL(clicked()), this, SLOT(startNewChannel()));
    connect(ui->deleteChannelButton, SIGNAL(clicked()), this, SLOT(deleteChannel()));
    connect(ui->refreshButton, SIGNAL(clicked()), serverCommunicator, SLOT(requestChannelList()));
    connect(this, SIGNAL(sendLogoutRequest()), serverCommunicator, SLOT(logout()));
    //******

    //initialize speaker class
    speakerWorker = new Speaker(settings);
    //signal emited when user starts or stops the playback
    connect(this, SIGNAL(broadcastStateChanged(QAudioFormat)), speakerWorker, SLOT(changeRecordState(QAudioFormat)));
    //recording state
    connect(speakerWorker, SIGNAL(recordingState(bool)), this, SLOT(changeBroadcastButtonState(bool)));
    //updating the ui with the traffic information
    connect(speakerWorker, SIGNAL(dataSent(int)), this, SLOT(setDataSent(int)));
    //stop the playback
    connect(this, SIGNAL(stopSpeaker()), speakerWorker, SLOT(stopRunning()));

    speakerThread = new QThread;
    //managing speaker thread lifetime
    connect(speakerWorker, SIGNAL(finished()), speakerThread, SLOT(quit()));
    connect(speakerWorker, SIGNAL(finished()), speakerWorker, SLOT(deleteLater()));
    connect(speakerWorker, SIGNAL(finished()), speakerThread, SLOT(deleteLater()));
    connect(speakerWorker, SIGNAL(errorMessage(QString)), this, SLOT(showErrorMessage(QString)));
    speakerWorker->moveToThread(speakerThread);

    //initialize listener class
    listenerWorker = new Listener(settings);
    listenerThread = new QThread;
    //playback state
    connect(this, SIGNAL(changePlayBackState( QSharedPointer<ChannelInfo> )), listenerWorker, SLOT(changePlaybackState( QSharedPointer<ChannelInfo> )));
    connect(listenerWorker, SIGNAL(changePlayButtonState(bool)), this, SLOT(changePlayButtonState(bool)));
    connect(this, SIGNAL(stopListener()), listenerWorker, SLOT(stopRunning()));
    //update gui with traffic information
    connect(listenerWorker, SIGNAL(dataReceived(int)), this, SLOT(setDataReceived(int)));
    //volume
    connect(this, SIGNAL(volumeChanged(int)), listenerWorker, SLOT(volumeChanged(int)));
    //change channel on doubleclick
    connect(ui->channelList, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(changeChannelOnDoubleClick(QModelIndex)));
    //record sound
    connect(this, SIGNAL(startRecord()), listenerWorker, SLOT(startRecord()));
    connect(listenerWorker, SIGNAL(changeRecordButtonState(RecordAudio::STATE)), this, SLOT(changeRecordButtonState(RecordAudio::STATE)));
    connect(listenerWorker, SIGNAL(changePauseButtonState(RecordAudio::STATE)), this, SLOT(changePauseButtonState(RecordAudio::STATE)));
    connect(this, SIGNAL(pauseRecord()), listenerWorker, SLOT(pauseRecord()));
    connect(listenerWorker, SIGNAL(errorMessage(QString)), this, SLOT(showErrorMessage(QString)));

    //manageing listener thread lifetime
    connect(listenerWorker, SIGNAL(finished()), listenerThread, SLOT(quit()));
    connect(listenerWorker, SIGNAL(finished()), listenerWorker, SLOT(deleteLater()));
    connect(listenerWorker, SIGNAL(finished()), listenerThread, SLOT(deleteLater()));
    connect(listenerWorker, SIGNAL(errorMessage(QString)), this, SLOT(showErrorMessage(QString)));
    listenerWorker->moveToThread(listenerThread);

    //model for channellist
    channelModel = new ChannelModel(this);
    ui->channelList->setModel(channelModel);
    connect(channelModel, SIGNAL(error(QString)), this, SLOT(showErrorMessage(QString)));
    connect(serverCommunicator, SIGNAL(serverList(QByteArray)), channelModel, SLOT(newChannelList(QByteArray)));
    connect(serverCommunicator, SIGNAL(channelConnected(ChannelInfo)), channelModel, SLOT(addNewChannel(ChannelInfo)));

     //add/remove channel from channellist(local channel, this will not start a new channel on server)
    addNewChannelMan = new AddNewChannelFromGui(settings->getOutputDevice(), this);
    connect(addNewChannelMan, SIGNAL(newUserCreatedChannel(ChannelInfo)), channelModel, SLOT(addNewUserCreatedChannel(ChannelInfo)));
    connect(serverCommunicator, SIGNAL(removeChannel(qint32)), channelModel, SLOT(deleteChannel(qint32)));

    //start threads
    speakerThread->start();
    listenerThread->start();

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

    //signals emitted when server is down
    connect(this, SIGNAL(stopPlaybackSD()), listenerWorker, SLOT(stopPlayback()));
    connect(this, SIGNAL(stopSpeakingSD()), speakerWorker, SLOT(stopRecording()));

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
            newChannelDialog->setClientId(settings->getClientId());
            newChannelDialog->setAudioDeviceInfo(settings->getOutputDevice());
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
    emit stopSpeakingSD();
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
    newChannelDialog->sendCloseChannelReq();
    ui->channelLangText->setText("-");
    ui->sampleRateText->setText("-");
    ui->sampleSizeText->setText("-");
    ui->channelNrText->setText("-");
    ui->codecText->setText("-");
    ui->newChannelBtn->setText("New channel");
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
    if(newChannelDialog->isChannelAvailable()) {
        QAudioFormat speakerFormat = newChannelDialog->getAudioFormat();
        emit broadcastStateChanged(speakerFormat);
    }
}

void GUI::changeBroadcastButtonState(bool isRecording) {
    if(isRecording) {
        ui->startBroadcastBtn->setText("Stop Broadcast");
        broadcastDataSize = 0;
        broadcastTimerStart();
    }
    else {
        ui->startBroadcastBtn->setText("Start Broadcast");
        broadcastDataSize = 0;
        broadcastTimerStop();
    }
}

//this SLOT is called when the Start button is pushed
void GUI::playbackButtonPushed() {
    QModelIndex selectedIndex = ui->channelList->currentIndex();
    try {
        QSharedPointer<ChannelInfo> selectedChannel = channelModel->getData(selectedIndex);
        emit changePlayBackState(selectedChannel);
    }
    catch(ChannelListException *ex) {
        QMessageBox msg;
        msg.setText(ex->message());
        msg.setStandardButtons(QMessageBox::Ok);
        msg.exec();
        delete ex;
    }
}

void GUI::changePlayButtonState(bool isPlaying) {
    if(isPlaying) {
        ui->playButton->setText("Stop");
        receiverTimerStart();
    }
    else {
        ui->playButton->setText("Play");
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
}

void GUI::addNewChannel() {
    addNewChannelMan->show();
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
    emit startRecord();
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

//at the end of the recording, users has the oportunity to set the file name of the recorded file
//in other case it will be saved as temporary(tmp) file
void GUI::setRecordFileName(QString filename) {
    bool renameOK = false;
    bool ok = true;
    QFile file(filename);
          while(!renameOK) {
              QString newName = QInputDialog::getText(this, tr("Save file as:"),
                                                      tr("Filename:"), QLineEdit::Normal,
                                                      tr(""), &ok);
              if(ok && !newName.isEmpty()) {
                  if(file.rename(settings->getRecordPath() + "/" + newName + ".wav")) {
                      renameOK = true;
                  }
                  else {
                       QMessageBox msgBox;
                       msgBox.setText("File could not saved.");
                       msgBox.setInformativeText("Would you like to enter a new filname or save it as a temporary(tmp.wav) file?");
                       QPushButton *tryAgain = msgBox.addButton(tr("Try Again"), QMessageBox::ActionRole);
                       QPushButton *save = msgBox.addButton(tr("Save as temporary"), QMessageBox::ActionRole);
                       QPushButton *del = msgBox.addButton(tr("Delete"), QMessageBox::ActionRole);

                       msgBox.exec();

                       if((QPushButton*)msgBox.clickedButton() == tryAgain) {
                           renameOK = false;
                       }
                       else {
                           if((QPushButton*)msgBox.clickedButton() == save) {
                               //save as temporary
                               renameOK = true;
                           }
                           else {
                               if((QPushButton*)msgBox.clickedButton() == del) {
                                   //delete recording
                                   renameOK = true;
                               }
                           }
                       }
                  }
              }
          }
          file.close();
}

//show any error message
void GUI::showErrorMessage(QString message) {
    QMessageBox msgBox;
    msgBox.setText(message);
    msgBox.exec();
}

void GUI::closeEvent(QCloseEvent *event) {

    if(listenerThread->isRunning() || speakerThread->isRunning()) {

        if(listenerThread->isRunning()) {

            emit sendLogoutRequest();

            emit stopListener();
            connect(listenerThread, SIGNAL(destroyed()), this, SLOT(close()));
            listenerThread->quit();
            event->ignore();
        }

        if(speakerThread->isRunning()) {
            emit stopSpeaker();
            connect(speakerThread, SIGNAL(destroyed()), this, SLOT(close()));
            speakerThread->quit();
            event->ignore();
        }
    }
    else {
        event->accept();
    }

}

//delete selected channel
void GUI::deleteChannel() {
    QModelIndex selectedIndex = ui->channelList->currentIndex();
    channelModel->deleteUserCreatedChannel(selectedIndex);
}


