#include "gui.h"
#include "ui_gui.h"

GUI::GUI(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GUI)
{
    ui->setupUi(this);
    connect(ui->startButton, SIGNAL(pressed()), this, SLOT(btn()));
    connect(ui->menuPreferences, SIGNAL(triggered(QAction*)), this, SLOT(menuTriggered(QAction*)));
    connect(ui->changeButton, SIGNAL(clicked()), this, SLOT(changeBroadcastingPort()));

    settings = new Settings(this);
    broadcasting_port = -1;
    broadcastDataSize = 0;
    broadcastDataPerSec = 0;
    cntBroadcastTime = 0;

    broadcastTimer.setInterval(1000);
    connect(&broadcastTimer, SIGNAL(timeout()), this, SLOT(updateBroadcastTime()));
}

GUI::~GUI()
{
    delete ui;
}

void GUI::setDataSent(int size) {
    broadcastDataSize += size;
    broadcastDataPerSec +=  size;
    ui->dataSent->setText(QString::number(broadcastDataSize / 1024) + "kByte");
}

void GUI::btn() {
    if(broadcasting_port == - 1) {
        QMessageBox msg;
        msg.setText("Invalid broadcasting port!");
        msg.setStandardButtons(QMessageBox::Ok);
        msg.exec();
        return;
    }
    QString btnText = ui->startButton->text();
    if(btnText.compare("Start Broadcast") == 0) {
        ui->startButton->setText("Stop Broadcast");
        broadcastTimerStart();
        emit startButtonPressed();
    }
    else {
        ui->startButton->setText("Start Broadcast");
        broadcastTimerStop();
        emit stopButtonPressed();
    }
}

Settings* GUI::getSettings() {
    return this->settings;
}

void GUI::menuTriggered(QAction* action) {
    if(action == ui->actionSettings) {
        settings->exec();
    }
    else {
        //exit
    }
}

void GUI::changeBroadcastingPort() {
    bool ok = true;
    int port = QInputDialog::getInt(this, tr("Change port"), tr("Insert port:"), 0, 1, 50000, 1, &ok);
    if(ok) {
        this->broadcasting_port = port;
        ui->portLabel->setText(QString::number(port));
    }
}

int GUI::getBroadcastingPort() {
    //QVariant data = ui->listWidget->currentItem()->data();
    return this->broadcasting_port;
}

void GUI::updateBroadcastTime() {
    cntBroadcastTime++;
    ui->timePassed->setText(QString::number(cntBroadcastTime) + " secs");
    ui->speed->setText(QString::number(broadcastDataPerSec / 1024) + " KB/s");
    broadcastDataPerSec = 0;
}

void GUI::broadcastTimerStart() {
    cntBroadcastTime = 0;
    broadcastTimer.start();
}

void GUI::broadcastTimerStop() {
    broadcastTimer.stop();
}

