#include "gui.h"
#include "ui_gui.h"

GUI::GUI(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GUI)
{
    ui->setupUi(this);
    this->channelCounter = 0;
    connect(ui->addButton, SIGNAL(clicked()), this, SLOT(startChannel()));
    connect(ui->deletButton, SIGNAL(clicked()), this, SLOT(stopChannel()));
}

GUI::~GUI()
{
    delete ui;
}

void GUI::startChannel() {
    channelCounter++;
    int portIn, portOut;
    QString port1 = ui->portIn->text();
    QString port2 = ui->portOut->text();
    bool ok = true;
    portIn = port1.toInt(&ok, 10);
    if(!ok) {
        QMessageBox msg;
        msg.setText("Invalid input for portIN!");
        msg.setStandardButtons(QMessageBox::Ok);
        msg.exec();
        return;
    }
    portOut = port2.toInt(&ok, 10);
    if(!ok) {
        QMessageBox msg;
        msg.setText("Invalid input for portOUT!");
        msg.setStandardButtons(QMessageBox::Ok);
        msg.exec();
        return;
    }
    AcceptData *channel = new AcceptData(portIn, portOut, this);
    channels.insert(channelCounter, channel);
    QListWidgetItem *item = new QListWidgetItem("Channel (" + port1 + " => " + port2 + ")");
    item->setData(Qt::UserRole, QVariant(channelCounter));
    ui->chList->addItem(item);
    channel->start();
}

void GUI::stopChannel() {
    QListWidgetItem *item = ui->chList->currentItem();
    QVariant data = item->data(Qt::UserRole);
    int index = data.toInt();
    int reply;
    reply = QMessageBox::question(this, "Delete Channel", "Do you really want to delete this channel", QMessageBox::Yes, QMessageBox::Cancel);
    if(reply == QMessageBox::Yes) {
        ui->chList->removeItemWidget(item);
        delete item;
        QMap<int, AcceptData*>::iterator it = channels.find(index);
        AcceptData *channel = it.value();
        //channel.stop();
        delete channel;
    }
    else {
        //
    }
}
