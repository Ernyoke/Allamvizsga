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

    //get settings
    settings = new Settings(this);

    //initialize timers
    timer.setInterval(1000);
    connect(&timer, SIGNAL(timeout()), this, SLOT(updateTime()));

    //initialize volume slider
    ui->volumeSlider->setMaximum(100);
    ui->volumeSlider->setValue(50);

    connect(ui->volumeSlider, SIGNAL(sliderMoved(int)), this, SLOT(volumeChangedSlot()));
    connect(ui->playButton, SIGNAL(clicked()), this, SLOT(playbackButtonPushed()));
    connect(ui->listWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(getItemData(QListWidgetItem*)));
    connect(ui->menuSettings, SIGNAL(triggered(QAction*)), this, SLOT(menuTriggered(QAction*)));
    connect(ui->newChannelButton, SIGNAL(pressed()), this, SLOT(addNewChannel()));

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

