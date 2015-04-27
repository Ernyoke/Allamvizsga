#include "newchanneldialog.h"
#include "ui_newchanneldialog.h"

NewChannelDialog::NewChannelDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewChannelDialog)
{
    ui->setupUi(this);

    connect(ui->startBtn, SIGNAL(clicked()), this, SLOT(setFormatProperties()));
    connect(ui->cancelBtn, SIGNAL(clicked()), this, SLOT(close()));

    this->socket = new QUdpSocket(this);

    this->isChannelOnline = false;

    timer = new QTimer(this);
    timer->setSingleShot(true);
    connect(timer, SIGNAL(timeout()), this, SLOT(newChannelRequestTimedOut()));
    creationTimeStamp = 0;
    chInfo = NULL;
}

NewChannelDialog::~NewChannelDialog()
{
    delete chInfo;
    delete ui;
}

void NewChannelDialog::displayDeviceProperties() {
    clearDeviceProperties();
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

void NewChannelDialog::clearDeviceProperties() {
    ui->sampleRateBox->clear();
    ui->channelBox->clear();
    ui->codecBox_2->clear();
    ui->sampleSizeBox->clear();
}

void NewChannelDialog::setFormatProperties() {
    if(!isChannelOnline) {

        language = ui->langInput->text().trimmed();

        if(language.isEmpty()) {
            ui->statusText->setText("Invalid language!");
            return;
        }

        formatSpeaker.setByteOrder(QAudioFormat::LittleEndian);
        formatSpeaker.setSampleType(QAudioFormat::UnSignedInt);

        formatSpeaker.setCodec(boxValue(ui->codecBox_2).toString());
        formatSpeaker.setSampleRate(boxValue(ui->sampleRateBox).toInt());
        formatSpeaker.setChannelCount(boxValue(ui->channelBox).toInt());
        formatSpeaker.setSampleSize(boxValue(ui->sampleSizeBox).toInt());

        delete chInfo;
        chInfo = new ChannelInfo(clientId, language, formatSpeaker.codec(), formatSpeaker.sampleRate(), formatSpeaker.sampleSize(), formatSpeaker.channelCount());
        sendNewChannelReq();

    }
    else {
        this->close();
    }
}

void NewChannelDialog::setClientId(qint32 id) {
    if(id <= 0) {
        InvalidIdException *exception = new InvalidIdException(this);
        exception->setMessage("Invalid id!");
        throw exception;
    }
    else {
        this->clientId = id;
    }
}

void NewChannelDialog::setAudioDeviceInfo(QAudioDeviceInfo deviceInfo) {
    this->deviceInfo = deviceInfo;
    displayDeviceProperties();
}

int NewChannelDialog::getBoxIndex(QComboBox *box, QString *content) {
    for(int i = 0; i < box->count(); ++i) {
        QString data = box->itemData(i, Qt::UserRole).toString();
        if(data.compare(*content) == 0) {
            return i;
        }
    }
    return -1;
}

QVariant NewChannelDialog::boxValue(const QComboBox *box)
{
    int idx = box->currentIndex();
    if (idx == -1)
        return QVariant();

    return box->itemData(idx);
}

QAudioFormat NewChannelDialog::getAudioFormat() {
    return this->formatSpeaker;
}

void NewChannelDialog::sendNewChannelReq() {
    creationTimeStamp = Datagram::generateTimestamp();
    Datagram dgram(Datagram::NEW_CHANNEL, clientId, creationTimeStamp);
    QByteArray content = chInfo->serialize();
    dgram.setDatagramContent(&content);
    emit requestNewChannel(dgram);
    timer->start(5000);

    ui->statusText->setText("Waiting for server response!");
}

void NewChannelDialog::newChannelAck(Datagram dgram) {
    if(dgram.getId() == Datagram::NEW_CHANNEL_ACK) {
        if(dgram.getTimeStamp() == creationTimeStamp) {
            timer->stop();
            isChannelOnline = true;
            ui->statusText->setText("Channel created succesful!");
            ui->startBtn->setText("Ok");
        }
    }
}

void NewChannelDialog::newChannelRequestTimedOut() {
    isChannelOnline = false;
    ui->statusText->setText("Server request timed out!");
    creationTimeStamp = 0;
}

bool NewChannelDialog::isChannelAvailable() {
    return this->isChannelOnline;
}

const ChannelInfo* NewChannelDialog::getChannelInformation() const {
    return this->chInfo;
}

void NewChannelDialog::sendCloseChannelReq() {
    if(isChannelOnline) {
        isChannelOnline = false;
        creationTimeStamp = Datagram::generateTimestamp();
        Datagram dgram(Datagram::CLOSE_CHANNEL, clientId, creationTimeStamp);
        QByteArray content = chInfo->serialize();
        dgram.setDatagramContent(&content);
        emit closeChannel(dgram);
    }
}
