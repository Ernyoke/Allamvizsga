#include "gui.h"
#include "ui_gui.h"

GUI::GUI(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GUI)
{
    ui->setupUi(this);

    settings = new Settings(this);
    speaker = new ManageVoice(this, settings);

    loginDialog = new LoginDialog(settings, this);

    connect(ui->startButton, SIGNAL(pressed()), this, SLOT(btn()));
    connect(ui->menuPreferences, SIGNAL(triggered(QAction*)), this, SLOT(menuTriggered(QAction*)));
    connect(ui->changeButton, SIGNAL(clicked()), this, SLOT(changeBroadcastingPort()));

    connect(speaker, SIGNAL(dataSent(int)), this, SLOT(setDataSent(int)));
    connect(speaker, SIGNAL(recordingState(bool)), this, SLOT(changeBroadcastButtonState(bool)));
    connect(this, SIGNAL(broadcastStateChanged(QString, QString)), speaker, SLOT(changeRecordState(QString, QString)));
    connect(speaker, SIGNAL(errorMessage(QString)), this, SLOT(errorMessage(QString)));

    connect(this, SIGNAL(logout()), loginDialog, SLOT(logout()));

    broadcasting_port = -1;
    broadcastDataSize = 0;
    broadcastDataPerSec = 0;
    cntBroadcastTime = 0;

    broadcastTimer.setInterval(1000);
    connect(&broadcastTimer, SIGNAL(timeout()), this, SLOT(updateBroadcastTime()));

    ui->statusBar->showMessage("Data transfer stopped!");
//    ui->menuBar->statusTip(
    login();
}

void GUI::login() {
    loginDialog->exec();
    if(!loginDialog->loginSucces()) {
        QTimer::singleShot(0, this, SLOT(close()));
    }
}

GUI::~GUI()
{
    delete ui;
    qDebug() << "GUI destruct!";
}

void GUI::setDataSent(int size) {
    broadcastDataSize += size;
    broadcastDataPerSec +=  size;
    ui->dataSent->setText(QString::number(broadcastDataSize / 1024) + "kByte");
}

void GUI::btn() {
    QString port = ui->portInput->text();
    QString address = ui->ipaddressInput->text();
    emit broadcastStateChanged(address, port);
}

void GUI::changeBroadcastButtonState(bool isRecording) {
    if(isRecording) {
        ui->startButton->setText("Stop Recording");
        broadcastDataSize = 0;
        ui->portLabel->setText(ui->portInput->text());
        ui->ipaddressLabel->setText(ui->ipaddressInput->text());
        broadcastTimerStart();
    }
    else {
        ui->startButton->setText("Start Recording");
        ui->statusBar->showMessage("Data transfer stopped!");
        broadcastDataSize = 0;
        broadcastTimerStop();
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
        return;
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
    ui->timePassed->setText(QString::number(cntBroadcastTime) + " seconds");
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

void GUI::errorMessage(QString message) {
    QMessageBox msg;
    msg.setText(message);
    msg.setStandardButtons(QMessageBox::Ok);
    msg.exec();
    return;
}

void GUI::closeEvent(QCloseEvent *event) {
   emit logout();
}


