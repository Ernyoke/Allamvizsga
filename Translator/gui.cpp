#include "gui.h"
#include "ui_gui.h"

GUI::GUI(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GUI)
{
    ui->setupUi(this);

    receiving = false;

    dataSize = 0;
    dataPerSec = 0;
    channels = new QList<QListWidgetItem*>();

    broadcastDataSize = 0;
    broadcastDataPerSec = 0;

    //broadcast port is not initialized yet
    broadcasting_port = -1;

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

    connect(ui->volumeSlider, SIGNAL(sliderMoved(int)), this, SLOT(volumeChangedSlot()));
    connect(ui->playButton, SIGNAL(clicked()), this, SLOT(playbackButtonPushed()));
    connect(ui->listWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(getItemData(QListWidgetItem*)));
    connect(ui->broadcastButton, SIGNAL(pressed()), this, SLOT(btn()));
    connect(ui->menuPreferences, SIGNAL(triggered(QAction*)), this, SLOT(menuTriggered(QAction*)));
    connect(ui->newChannelButton, SIGNAL(pressed()), this, SLOT(addNewChannel()));
    connect(ui->portChange, SIGNAL(clicked()), this, SLOT(changeBroadcastingPort()));

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

void GUI::setDataSent(int size) {
    broadcastDataSize += size;
    broadcastDataPerSec +=  size;
    ui->dataSent->setText(QString::number(broadcastDataSize / 1024) + "kByte");
}

void GUI::playbackButtonPushed() {
    if(!receiving) {
        QListWidgetItem *item = ui->listWidget->currentItem();
        if(item != NULL) {
            ui->playButton->setText("Stop");
            receiving = true;
            receiverTimerStart();
            QVariant data = item->data(Qt::UserRole);
            int port = data.toInt();
            emit portChanged(port);
        }
        else {
            QMessageBox msg;
            msg.setText("No channel was selected!");
            msg.setStandardButtons(QMessageBox::Ok);
            msg.exec();
            receiving = false;
        }
    }
    else {
        ui->playButton->setText("Play");
        receiverTimerStop();
        emit stopPlayback();
        receiving = false;
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

void GUI::volumeChangedSlot() {
    emit volumeChanged();
}

void GUI::getItemData(QListWidgetItem *item) {
    receiving = false;
    this->playbackButtonPushed();
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
    QString btnText = ui->broadcastButton->text();
    if(btnText.compare("Start Broadcast") == 0) {
        ui->broadcastButton->setText("Stop Broadcast");
        broadcastTimerStart();
        emit startButtonPressed();
    }
    else {
        ui->broadcastButton->setText("Start Broadcast");
        broadcastTimerStop();
        emit stopButtonPressed();
    }
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

int GUI::getBroadcastingPort() {
    //QVariant data = ui->listWidget->currentItem()->data();
    return this->broadcasting_port;
}

void GUI::menuTriggered(QAction* action) {
    if(action == ui->actionSettings) {
        settings->exec();
    }
    else {
        //exit
    }
}

Settings* GUI::getSettings() {
    return settings;
}


