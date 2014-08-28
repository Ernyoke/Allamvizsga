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

    //get settings
    settings = new Settings(this);

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


