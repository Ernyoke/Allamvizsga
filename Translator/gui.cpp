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

    broadcastDataSize = 0;
    broadcastDataPerSec = 0;

    //get settings
    settings = new Settings(this);

    //initialize timers
    timer.setInterval(1000);
    connect(&timer, SIGNAL(timeout()), this, SLOT(updateTime()));

    broadcastTimer.setInterval(1000);
    connect(&broadcastTimer, SIGNAL(timeout()), this, SLOT(updateBroadcastTime()));

    //initialize volume slider
    ui->volumeSlider->setMaximum(100);
    ui->volumeSlider->setValue(50);

    //initialize statusbar
    ui->statusBar->showMessage("Player stopped!");

    connect(ui->volumeSlider, SIGNAL(sliderMoved(int)), this, SLOT(volumeChangedSlot()));
    connect(ui->playButton, SIGNAL(clicked()), this, SLOT(playbackButtonPushed()));
    connect(ui->listWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(getItemData(QListWidgetItem*)));
    connect(ui->newChannelButton, SIGNAL(pressed()), this, SLOT(addNewChannel()));
    connect(ui->recordButton, SIGNAL(clicked()), this, SLOT(startRecordPushed()));
    connect(ui->pauseRec, SIGNAL(clicked()), this, SLOT(pauseRecordPushed()));

    connect(ui->broadcastButton, SIGNAL(pressed()), this, SLOT(btn()));
    connect(ui->menuPreferences, SIGNAL(triggered(QAction*)), this, SLOT(menuTriggered(QAction*)));
    connect(ui->portChange, SIGNAL(clicked()), this, SLOT(changeBroadcastingPort()));

}

GUI::~GUI()
{
    delete ui;
    qDebug() << "GUI destructor!";
}

void GUI::setDataReceived(int size) {
    dataSize += size;
    ui->dataReceived->setText(QString::number(dataSize / 1024) + "kByte");
    dataPerSec += size;
}

void GUI::setDataSent(int size) {
    broadcastDataSize += size;
    broadcastDataPerSec +=  size;
    ui->dataSent->setText(QString::number(broadcastDataSize / 1024) + "kByte");
}

//this SLOT is called when the Start button is pushed
void GUI::playbackButtonPushed() {
        QListWidgetItem *item = ui->listWidget->currentItem();
        if(item != NULL) {
            emit changePlayBackState();
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

void GUI::changeBroadcastButtonState(bool isRecording) {
    if(isRecording) {
        ui->broadcastButton->setText("Stop Recording");
        ui->statusBar->showMessage("Transfering data on port: " + QString::number(broadcasting_port));
        broadcastDataSize = 0;
        broadcastTimerStart();
    }
    else {
        ui->broadcastButton->setText("Start Recording");
        ui->statusBar->showMessage("Data transfer stopped!");
        broadcastDataSize = 0;
        broadcastTimerStop();
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

void GUI::changeBroadcastingPort() {
    bool ok = true;
    int port = QInputDialog::getInt(this, tr("Change port"), tr("Insert port:"), 0, 1, 50000, 1, &ok);
    if(ok) {
        this->broadcasting_port = port;
        ui->portLabel->setText(QString::number(port));
    }
}

void GUI::btn() {
    if(broadcasting_port == - 1) {
        QMessageBox msg;
        msg.setText("Invalid broadcasting port!");
        msg.setStandardButtons(QMessageBox::Ok);
        msg.exec();
        return;
    }
   emit broadcastStateChanged();
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
    if(action == ui->actionSettings) {
        settings->exec();
    }
    else {
        //exit
    }
}

int GUI::getBroadcastingPort() {
    //QVariant data = ui->listWidget->currentItem()->data();
    return this->broadcasting_port;
}

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


