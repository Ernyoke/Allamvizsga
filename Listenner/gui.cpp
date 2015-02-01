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
    channels = new QList<QListWidgetItem*>();

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
    connect(ui->listWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(getItemData(QListWidgetItem*)));
    connect(ui->menuSettings, SIGNAL(triggered(QAction*)), this, SLOT(menuTriggered(QAction*)));
    connect(ui->newChannelButton, SIGNAL(pressed()), this, SLOT(addNewChannel()));
    connect(ui->recordButton, SIGNAL(clicked()), this, SLOT(startRecordPushed()));
    connect(ui->pauseRec, SIGNAL(clicked()), this, SLOT(pauseRecordPushed()));

    connect(this, SIGNAL(changePlayBackState(int)), listener, SLOT(changePlaybackState(int)));
    connect(listener, SIGNAL(changePlayButtonState(bool)), this, SLOT(changePlayButtonState(bool)));
    connect(this, SIGNAL(volumeChanged()), listener, SLOT(volumeChanged()));
    connect(this, SIGNAL(portChanged(int)), listener, SLOT(portChanged(int)));
    connect(this, SIGNAL(startRecord()), listener, SLOT(startRecord()));
    connect(this, SIGNAL(pauseRecord()), listener, SLOT(pauseRecord()));
    connect(listener, SIGNAL(dataReceived(int)), this, SLOT(setDataReceived(int)));
    connect(listener, SIGNAL(askFileNameGUI(QString)), this, SLOT(setRecordFileName(QString)));
    connect(listener, SIGNAL(showError(QString)), this, SLOT(showErrorMessage(QString)));
    connect(listener, SIGNAL(changeRecordButtonState(RecordAudio::STATE)), this, SLOT(changeRecordButtonState(RecordAudio::STATE)));
    connect(listener, SIGNAL(changePauseButtonState(RecordAudio::STATE)), this, SLOT(changePauseButtonState(RecordAudio::STATE)));
    connect(ui->deleteChannelButton, SIGNAL(clicked()), this, SLOT(deleteChannel()));

    login();
}

void GUI::login() {
    LoginDialog login(this->settings);
    login.login();
    login.exec();
    if(!login.loginSucces()) {
        QTimer::singleShot(0, this, SLOT(close()));
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
        QListWidgetItem *item = ui->listWidget->currentItem();
        broadcasting_port = this->getPort();
        if(item != NULL) {
            emit changePlayBackState(broadcasting_port);
        }
        else {
            QMessageBox msg;
            msg.setText("No channel was selected!");
            msg.setStandardButtons(QMessageBox::Ok);
            msg.exec();
        }
}

void GUI::changePlayButtonState(bool isPlaying) {
    if(isPlaying) {
        ui->playButton->setText("Stop");
        ui->statusBar->clearMessage();
        QListWidgetItem *item = ui->listWidget->currentItem();
        QVariant data = item->data(Qt::UserRole);
        int port = data.toInt();
        ui->statusBar->showMessage("Ongoing playback! Recieving data from port: " + QString::number(port));
        receiverTimerStart();
    }
    else {
        ui->playButton->setText("Play");
        ui->statusBar->clearMessage();
        ui->statusBar->showMessage("Playback stopped!");
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

int GUI::getPort() {
    QListWidgetItem *item = ui->listWidget->currentItem();
    QVariant data = item->data(Qt::UserRole);
    int port = data.toInt();
    return port;
}

void GUI::volumeChangedSlot() {
    emit volumeChanged();
}

void GUI::getItemData(QListWidgetItem *item) {
    QVariant data = item->data(Qt::UserRole);
    int port = data.toInt();
    emit portChanged(port);
}


void GUI::addNewChannel() {
    bool ok = true;
    int port = QInputDialog::getInt(this, tr("Add new channel"), tr("Insert port:"), 0, 1, 50000, 1, &ok);
    if(ok) {
        QListWidgetItem *item1 = new QListWidgetItem("Port " + QString::number(port));
        item1->setData(Qt::UserRole, QVariant(port));
        channels->append(item1);
        ui->listWidget->addItem(item1);
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
   QListWidgetItem *item = ui->listWidget->currentItem();
   int nr = ui->listWidget->currentRow();
   ui->listWidget->removeItemWidget(item);
   channels->removeAt(nr);
   delete item;
}


