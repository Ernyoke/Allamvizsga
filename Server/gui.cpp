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

    manageClients = new ManageClients();
}

GUI::~GUI()
{
    delete ui;
}

//starts a new channel
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
    if(this->checkUsedPorts(portIn, portOut)) {
        AcceptData *channel = new AcceptData(portIn, portOut, this);
        channels.insert(channelCounter, channel);
        QListWidgetItem *item = new QListWidgetItem("Channel (" + port1 + " => " + port2 + ")");
        item->setData(Qt::UserRole, QVariant(channelCounter));
        ui->chList->addItem(item);
    }
}

//stops and delets a channel from running
void GUI::stopChannel() {
    //check if list has any item
    if(ui->chList->count() > 0) {
        QListWidgetItem *item = ui->chList->currentItem();
         //check if is any item selected
        if(item != NULL) {
            QVariant data = item->data(Qt::UserRole);
            int index = data.toInt();
            int reply;
            reply = QMessageBox::question(this, "Delete Channel", "Do you really want to delete this channel", QMessageBox::Yes, QMessageBox::Cancel);
            if(reply == QMessageBox::Yes) {
                ui->chList->removeItemWidget(item);
                delete item;
                QMap<int, AcceptData*>::iterator it = channels.find(index);
                AcceptData *channel = it.value();
                delete channel;
            }
            else {
                //
            }
        }
    }
}

//checks if inserted in/out ports are available
bool GUI::checkUsedPorts(int portIn, int portOut) {
    for(QMap<int, AcceptData*>::iterator it = channels.begin(); it != channels.end(); ++it) {
        if((*it)->getPortIn() == portIn || (*it)->getPortOut() == portIn || (*it)->getPortIn() == portOut || (*it)->getPortOut() == portOut) {
            QMessageBox msg;
            msg.setText("A port is already used!");
            msg.setStandardButtons(QMessageBox::Ok);
            msg.exec();
            return false;
        }
    }
    return true;
}

void GUI::keyPressEvent(QKeyEvent *key) {
    switch(key->key()) {
    //adds new channel when the Enter key is pressed
    case Qt::Key_Enter: {
        this->startChannel();
        break;
    }
    //delets selected channel when Delete key is pressed
    case Qt::Key_Delete: {
        this->stopChannel();
        break;
    }
    }
}
