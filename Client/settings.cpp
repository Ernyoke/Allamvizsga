#include "settings.h"
#include "ui_settings.h"

Settings::Settings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Settings)
{
    ui->setupUi(this);

    QAudioDeviceInfo info = QAudioDeviceInfo::defaultInputDevice();
    foreach (const int it, info.supportedSampleRates()) {
            ui->sampleRateBox->addItem(QString::number(it), QVariant(it));
    }
    foreach (const int it, info.supportedChannelCounts()) {
            ui->channelBox->addItem(QString::number(it), QVariant(it));
    }
    foreach (const QString &it, info.supportedCodecs()) {
            ui->codecBox_2->addItem(it, QVariant(it));
    }

    foreach (const int it, info.supportedSampleSizes()) {
            ui->sampleSizeBox->addItem(QString::number(it), QVariant(it));
    }

    ui->sampleSizeBox->setCurrentIndex(1); //*******//

    formatSpeaker = new QAudioFormat;

    applySettings();
    connect(ui->applyButton, SIGNAL(clicked()), this, SLOT(applySettings()));
    connect(ui->cancelButton, SIGNAL(clicked()), this, SLOT(cancelSetting()));

}

Settings::~Settings()
{
    delete ui;


}

QAudioFormat* Settings::getSpeakerAudioFormat() {
    return formatSpeaker;
}


QVariant Settings::boxValue(const QComboBox *box)
{
    int idx = box->currentIndex();
    if (idx == -1)
        return QVariant();

    return box->itemData(idx);
}

void Settings::applySettings() {
    formatSpeaker->setByteOrder(QAudioFormat::LittleEndian);
    formatSpeaker->setSampleType(QAudioFormat::UnSignedInt);

    formatSpeaker->setCodec(boxValue(ui->codecBox_2).toString());
    formatSpeaker->setSampleRate(boxValue(ui->sampleRateBox).toInt());
    formatSpeaker->setChannelCount(boxValue(ui->channelBox).toInt());
    formatSpeaker->setSampleSize(boxValue(ui->sampleSizeBox).toInt());
}

void Settings::cancelSetting() {
    this->close();
}
