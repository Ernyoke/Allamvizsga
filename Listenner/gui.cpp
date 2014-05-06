#include "gui.h"
#include "ui_gui.h"

GUI::GUI(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GUI)
{
    ui->setupUi(this);
    dataSize = 0;
    receiving = false;
    connect(ui->playButton, SIGNAL(clicked()), this, SLOT(playbackButtonPushed()));

    timer.setInterval(1000);
    connect(&timer, SIGNAL(timeout()), this, SLOT(updateTime()));

    ui->volumeSlider->setMaximum(100);
    ui->volumeSlider->setValue(50);

    connect(ui->volumeSlider, SIGNAL(sliderMoved(int)), this, SLOT(volumeChangedSlot()));

    channels = new QList<QListWidgetItem*>();

    QListWidgetItem *item1 = new QListWidgetItem("Port 45000");
    item1->setData(Qt::UserRole, QVariant(45000));
    channels->append(item1);

    QListWidgetItem *item2 = new QListWidgetItem("Port 45001");
    item2->setData(Qt::UserRole, QVariant(45001));
    channels->append(item2);

    QListWidgetItem *item3 = new QListWidgetItem("Port 45002");
    item3->setData(Qt::UserRole, QVariant(45002));
    channels->append(item3);

    for(QList<QListWidgetItem*>::iterator it = channels->begin(); it != channels->end(); ++it) {
        ui->listWidget->addItem(*it);
    }

    connect(ui->listWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(getItemData(QListWidgetItem*)));
}

GUI::~GUI()
{
    delete ui;
}

void GUI::setDataReceived(int size) {
    dataSize += size;
    ui->dataReceived->setText(QString::number(dataSize));
}

void GUI::playbackButtonPushed() {
    if(!receiving) {
        ui->playButton->setText("Stop");
        receiving = true;
        timerStart();
        emit startPlayback();
    }
    else {
        ui->playButton->setText("Play");
        timerStop();
        emit stopPlayback();
        receiving = false;
    }
}

void GUI::timerStart() {
    cntTime = 0;
    timer.start();
}

void GUI::timerStop() {
    timer.stop();
}

void GUI::updateTime() {
    cntTime++;
    ui->timePassed->setText(QString::number(cntTime) + "secs");
}

int GUI::getVolume() {
    return ui->volumeSlider->value();
}

void GUI::volumeChangedSlot() {
    emit volumeChanged();
}

void GUI::getItemData(QListWidgetItem *item) {
    QVariant data = item->data(Qt::UserRole);
    int port = data.toInt();
    emit portChanged(port);
}
