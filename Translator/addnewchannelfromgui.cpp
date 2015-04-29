#include "addnewchannelfromgui.h"
#include "ui_addnewchannelfromgui.h"

AddNewChannelFromGui::AddNewChannelFromGui(QAudioDeviceInfo deviceInfo, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddNewChannelFromGui)
{
    ui->setupUi(this);

    this->deviceInfo = deviceInfo;
    this->displayDeviceProperties();

    connect(ui->startBtn, SIGNAL(clicked()), this, SLOT(createNewChannel()));
    connect(ui->cancelBtn, SIGNAL(clicked()), this, SLOT(close()));

    startBtnStatus = true;
    connect(ui->codecBox_2, SIGNAL(currentIndexChanged(int)), this, SLOT(fieldChannged()));
    connect(ui->sampleRateBox, SIGNAL(currentIndexChanged(int)), this, SLOT(fieldChannged()));
    connect(ui->sampleSizeBox, SIGNAL(currentIndexChanged(int)), this, SLOT(fieldChannged()));
    connect(ui->channelBox, SIGNAL(currentIndexChanged(int)), this, SLOT(fieldChannged()));
    connect(ui->langInput, SIGNAL(textChanged(QString)), this, SLOT(fieldChannged()));
    connect(ui->portInput, SIGNAL(textChanged(QString)), this, SLOT(fieldChannged()));

}

AddNewChannelFromGui::~AddNewChannelFromGui()
{
    delete ui;
}

void AddNewChannelFromGui::displayDeviceProperties() {
    foreach (const int it, deviceInfo.supportedSampleRates()) {
        if(it <= 44100) {
            ui->sampleRateBox->addItem(QString::number(it) + " Hz", QVariant(it));
        }
    }
    foreach (const int it, deviceInfo.supportedChannelCounts()) {
        if(it == 1) {
            ui->channelBox->addItem(QString("mono"), QVariant(it));
        }
        if(it == 2) {
            ui->channelBox->addItem(QString("stereo"), QVariant(it));
        }
    }
    foreach (const QString &it, deviceInfo.supportedCodecs()) {
            ui->codecBox_2->addItem(it, QVariant(it));
    }

    foreach (const int it, deviceInfo.supportedSampleSizes()) {
            ui->sampleSizeBox->addItem(QString::number(it) + " bits", QVariant(it));
    }
}

void AddNewChannelFromGui::createNewChannel() {
    if(startBtnStatus) {
        QString language = ui->langInput->text().trimmed();
        if(language.isEmpty()) {
            ui->statusText->setText("Language field should not be empty!");
            return;
        }
        bool ok = true;
        ChannelInfo chInfo(0, language, boxValue(ui->codecBox_2).toString(),
                           boxValue(ui->sampleRateBox).toInt(), boxValue(ui->sampleSizeBox).toInt(), boxValue(ui->channelBox).toInt());
        chInfo.setOutPort(ui->portInput->text().toInt(&ok));
        if(ok) {
            qDebug() << chInfo.getOutPort();
            ui->statusText->setText("Channel created");
            ui->startBtn->setText("Ok");
            startBtnStatus = false;
            emit newUserCreatedChannel(chInfo);
        }
        else {
            ui->statusText->setText("Invalid port!");
        }
    }
    else {
        this->close();
    }
}

int AddNewChannelFromGui::getBoxIndex(QComboBox *box, QString *content) {
    for(int i = 0; i < box->count(); ++i) {
        QString data = box->itemData(i, Qt::UserRole).toString();
        if(data.compare(*content) == 0) {
            return i;
        }
    }
    return -1;
}

QVariant AddNewChannelFromGui::boxValue(const QComboBox *box)
{
    int idx = box->currentIndex();
    if (idx == -1)
        return QVariant();

    return box->itemData(idx);
}

void AddNewChannelFromGui::fieldChannged() {
    startBtnStatus = true;
    ui->startBtn->setText("Add channel");
}
