#include "gui.h"
#include "ui_gui.h"

GUI::GUI(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GUI)
{
    ui->setupUi(this);
    initialize();

}


void GUI::initialize() {

    dataSize = 0;
    dataPerSec = 0;

    broadcasting_port = -1;

    //get settings
    settings = new Settings(this);

    //
    listener = new Listener(this, settings);

    //initialize timers
    timer.setInterval(1000);
    connect(&timer, SIGNAL(timeout()), this, SLOT(updateTime()));

    //initialize volume slider
    ui->volumeSlider->setMaximum(100);
    ui->volumeSlider->setValue(50);

    //initialize statusbar
    ui->statusBar->showMessage("Player stopped!");

    connect(ui->volumeSlider, SIGNAL(sliderMoved(int)), this, SLOT(volumeChangedSlot()));
    connect(ui->playButton, SIGNAL(clicked()), this, SLOT(playbackButtonPushed()));
    connect(ui->channelList, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(changeChannelOnDoubleClick(QModelIndex)));
    connect(ui->menuSettings, SIGNAL(triggered(QAction*)), this, SLOT(menuTriggered(QAction*)));
    connect(ui->newChannelButton, SIGNAL(clicked()), this, SLOT(addNewChannel()));
    connect(ui->recordButton, SIGNAL(clicked()), this, SLOT(startRecordPushed()));
    connect(ui->pauseRec, SIGNAL(clicked()), this, SLOT(pauseRecordPushed()));

    connect(this, SIGNAL(changePlayBackState( QSharedPointer<ChannelInfo> )), listener, SLOT(changePlaybackState( QSharedPointer<ChannelInfo> )));
    connect(listener, SIGNAL(changePlayButtonState(bool)), this, SLOT(changePlayButtonState(bool)));
    connect(this, SIGNAL(volumeChanged()), listener, SLOT(volumeChanged()));
    connect(this, SIGNAL(startRecord()), listener, SLOT(startRecord()));
    connect(this, SIGNAL(pauseRecord()), listener, SLOT(pauseRecord()));
    connect(listener, SIGNAL(dataReceived(int)), this, SLOT(setDataReceived(int)));
    connect(listener, SIGNAL(askFileNameGUI(QString)), this, SLOT(setRecordFileName(QString)));
    connect(listener, SIGNAL(showError(QString)), this, SLOT(showErrorMessage(QString)));
    connect(listener, SIGNAL(changeRecordButtonState(RecordAudio::STATE)), this, SLOT(changeRecordButtonState(RecordAudio::STATE)));
    connect(listener, SIGNAL(changePauseButtonState(RecordAudio::STATE)), this, SLOT(changePauseButtonState(RecordAudio::STATE)));
    connect(ui->deleteChannelButton, SIGNAL(clicked()), this, SLOT(deleteChannel()));

    serverCommunicator = new ServerCommunicator(settings, this);
    loginDialog = new LoginDialog(settings, this);
    connect(loginDialog, SIGNAL(sendLoginRequest(Datagram)), serverCommunicator, SLOT(sendLoginRequest(Datagram)));
    connect(loginDialog, SIGNAL(sendLogoutRequest(Datagram)), serverCommunicator, SLOT(sendDatagram(Datagram)));
    connect(loginDialog, SIGNAL(sendLoginResponse(Datagram)), serverCommunicator, SLOT(sendDatagram(Datagram)));
    connect(this, SIGNAL(logout()), loginDialog, SLOT(logout()));
    connect(serverCommunicator, SIGNAL(loginAckReceived(Datagram)), loginDialog, SLOT(processLogin(Datagram)));

    channelModel = new ChannelModel(this);
    ui->channelList->setModel(channelModel);
    connect(serverCommunicator, SIGNAL(serverList(QByteArray)), channelModel, SLOT(newChannelList(QByteArray)));

//    login();
    addNewChannelMan = new AddNewChannelFromGui(settings->getOutputDevice(), this);
    connect(addNewChannelMan, SIGNAL(newUserCreatedChannel(ChannelInfo)), channelModel, SLOT(addNewUserCreatedChannel(ChannelInfo)));
}

void GUI::login() {
    loginDialog->exec();
    if(!loginDialog->loginSucces()) {
        QTimer::singleShot(0, this, SLOT(close()));
    }
    else {
        this->show();
         serverCommunicator->requestChannelList();
    }
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


int GUI::getVolume() {
    return ui->volumeSlider->value();
}

void GUI::volumeChangedSlot() {
    emit volumeChanged();
}

void GUI::changeChannelOnDoubleClick(QModelIndex index) {
    qDebug() << index.row();
}


void GUI::addNewChannel() {
    addNewChannelMan->show();
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

            event->accept();
        }
        else {
            event->ignore();
        }
    }
    else {
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


