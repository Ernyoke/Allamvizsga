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
    connect(loginDialog, SIGNAL(sendLoginRequest(Datagram)), serverCommunicator, SLOT(sendLoginRequest(Datagram)));
    //logout request passed from logindialog to servercommunicator
    connect(loginDialog, SIGNAL(sendLogoutRequest(Datagram)), serverCommunicator, SLOT(sendDatagram(Datagram)));
    //login acknoledged signal passed from logindialog to servercommunicator
    connect(loginDialog, SIGNAL(sendLoginResponse(Datagram)), serverCommunicator, SLOT(sendDatagram(Datagram)));
    //logout signal from gui, this is emited when the user quits the program
    connect(this, SIGNAL(logout()), loginDialog, SLOT(logout()));
    //server login response passed to logindialog
    connect(serverCommunicator, SIGNAL(loginAckReceived(Datagram)), loginDialog, SLOT(processLogin(Datagram)));


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
    //******

    //initialize speaker class
    speaker = new Speaker(settings);
    //signal emited when user starts or stops the playback
    connect(this, SIGNAL(broadcastStateChanged(QAudioFormat)), speaker, SLOT(changeRecordState(QAudioFormat)));
    //recording state
    connect(speaker, SIGNAL(recordingState(bool)), this, SLOT(changeBroadcastButtonState(bool)));
    //updating the ui with the traffic information
    connect(speaker, SIGNAL(dataSent(int)), this, SLOT(setDataSent(int)));
    //stop the playback
    connect(this, SIGNAL(stopSpeaker()), speaker, SLOT(stopRunning()));

    threadSpeaker = new QThread;
    //managing speaker thread lifetime
    connect(speaker, SIGNAL(finished()), threadSpeaker, SLOT(quit()));
    connect(speaker, SIGNAL(finished()), speaker, SLOT(deleteLater()));
    connect(speaker, SIGNAL(errorMessage(QString)), this, SLOT(showErrorMessage(QString)));
    speaker->moveToThread(threadSpeaker);

    //initialize listener class
    listener = new Listener(settings);
    threadListener = new QThread;
    //playback state
    connect(this, SIGNAL(changePlayBackState( QSharedPointer<ChannelInfo> )), listener, SLOT(changePlaybackState( QSharedPointer<ChannelInfo> )));
    connect(listener, SIGNAL(changePlayButtonState(bool)), this, SLOT(changePlayButtonState(bool)));
    connect(this, SIGNAL(stopListener()), listener, SLOT(stopRunning()));
    //update gui with traffic information
    connect(listener, SIGNAL(dataReceived(int)), this, SLOT(setDataReceived(int)));
    //volume changed
    connect(this, SIGNAL(volumeChanged(int)), listener, SLOT(volumeChanged(int)));
    connect(ui->channelList, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(changeChannelOnDoubleClick(QModelIndex)));
    //recording sound
    connect(this, SIGNAL(startRecord()), listener, SLOT(startRecord()));
    connect(listener, SIGNAL(changeRecordButtonState(RecordAudio::STATE)), this, SLOT(changeRecordButtonState(RecordAudio::STATE)));
    connect(listener, SIGNAL(changePauseButtonState(RecordAudio::STATE)), this, SLOT(changePauseButtonState(RecordAudio::STATE)));
    connect(this, SIGNAL(pauseRecord()), listener, SLOT(pauseRecord()));
    connect(listener, SIGNAL(askFileNameGUI(QString)), this, SLOT(setRecordFileName(QString)), Qt::BlockingQueuedConnection);
    connect(listener, SIGNAL(showError(QString)), this, SLOT(showErrorMessage(QString)));

    //manageing listener thread lifetime
    connect(listener, SIGNAL(finished()), threadListener, SLOT(quit()));
    connect(listener, SIGNAL(finished()), listener, SLOT(deleteLater()));
    listener->moveToThread(threadListener);


    channelModel = new ChannelModel(this);
    ui->channelList->setModel(channelModel);
    connect(serverCommunicator, SIGNAL(serverList(QByteArray)), channelModel, SLOT(newChannelList(QByteArray)));
    connect(serverCommunicator, SIGNAL(channelConnected(ChannelInfo)), channelModel, SLOT(addNewChannel(ChannelInfo)));

    addNewChannelMan = new AddNewChannelFromGui(settings->getOutputDevice(), this);
    connect(addNewChannelMan, SIGNAL(newUserCreatedChannel(ChannelInfo)), channelModel, SLOT(addNewUserCreatedChannel(ChannelInfo)));

    //start threads
    threadSpeaker->start();
    threadListener->start();

    //initialize timers
    timer.setInterval(1000);
    connect(&timer, SIGNAL(timeout()), this, SLOT(updateTime()));

    broadcastTimer.setInterval(1000);
    connect(&broadcastTimer, SIGNAL(timeout()), this, SLOT(updateBroadcastTime()));

}

void GUI::login() {
    loginDialog->exec();
    if(!loginDialog->loginSucces()) {
        QTimer::singleShot(0, this, SLOT(close()));
    }
    else {
        this->show();
        newChannelDialog = new NewChannelDialog(settings->getClientId(), settings->getOutputDevice(), this);
        connect(newChannelDialog, SIGNAL(requestNewChannel(Datagram)), serverCommunicator, SLOT(sendDatagram(Datagram)));
        connect(serverCommunicator, SIGNAL(newChannelAckReceived(Datagram)), newChannelDialog, SLOT(newChannelAck(Datagram)));
        connect(newChannelDialog, SIGNAL(closeChannel(Datagram)), serverCommunicator, SLOT(sendDatagram(Datagram)));

        serverCommunicator->requestChannelList();
    }
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
        newChannelDialog->sendCloseChannelReq();
        ui->channelLangText->setText("-");
        ui->sampleRateText->setText("-");
        ui->sampleSizeText->setText("-");
        ui->channelNrText->setText("-");
        ui->codecText->setText("-");
        ui->newChannelBtn->setText("New channel");
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

void GUI::changeBroadcastingPort() {
    bool ok = true;
    int port = QInputDialog::getInt(this, tr("Change port"), tr("Insert port:"), 0, 1, 50000, 1, &ok);
    if(ok) {
        this->broadcasting_port = port;
        ui->portLabel->setText(QString::number(port));
    }
}

void GUI::addNewChannel() {
    addNewChannelMan->show();
}

void GUI::menuTriggered(QAction* action) {
    if(action == ui->actionSettings) {
        settings->exec();
    }
    else {
        //exit
    }
}

int GUI::getBroadcastingPort() {
    return this->broadcasting_port;
}

//show setting window
void GUI::showSettings() {
    settings->exec();
}

Settings* GUI::getSettings() {
    return settings;
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
    emit logout();
    if(listener->isRecRunning()) {
        QMessageBox msgBox;
        msgBox.setText("Recording is still in progress.");
        QPushButton *exit = msgBox.addButton(tr("Exit without saving"), QMessageBox::ActionRole);
        QPushButton *save = msgBox.addButton(tr("Close"), QMessageBox::ActionRole);

        msgBox.exec();

        if((QPushButton*)msgBox.clickedButton() == exit) {

            disconnect(listener, SIGNAL(askFileNameGUI(QString)), this, SLOT(setRecordFileName(QString)));

            emit stopSpeaker();
            emit stopListener();
            event->accept();
        }
        else {
            event->ignore();
        }
    }
    else {
        emit stopSpeaker();
        emit stopListener();
        event->accept();
    }
}

//delete selected channel
void GUI::deleteChannel() {
//   QListWidgetItem *item = ui->listWidget->currentItem();
//   int nr = ui->listWidget->currentRow();
//   ui->listWidget->removeItemWidget(item);
//   channels->removeAt(nr);
//   delete item;
}


