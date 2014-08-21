#include "settings.h"
#include "ui_settings.h"

Settings::Settings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Settings)
{
    ui->setupUi(this);

    selectedInputDevice = QAudioDeviceInfo::defaultInputDevice();
    selectedOutputDevice = QAudioDeviceInfo::defaultOutputDevice();

    input_devices = QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
    foreach (const QAudioDeviceInfo it,input_devices) {
            ui->inputDeviceBox->addItem(it.deviceName(), QVariant(it.deviceName()));
    }

    output_devices = QAudioDeviceInfo::availableDevices(QAudio::AudioOutput);
    foreach (const QAudioDeviceInfo it,output_devices) {
            ui->outputDeviceBox->addItem(it.deviceName(), QVariant(it.deviceName()));
    }

    displayInputDeviceProperties(selectedInputDevice);
    displayOutputDeviceProperties(selectedOutputDevice);

    applySettings();
    connect(ui->applyButton, SIGNAL(clicked()), this, SLOT(applySettings()));
    connect(ui->cancelButton, SIGNAL(clicked()), this, SLOT(cancelSetting()));

}

Settings::~Settings()
{
    delete ui;
}

void Settings::displayInputDeviceProperties(QAudioDeviceInfo device) {
    foreach (const int it, device.supportedSampleRates()) {
            ui->sampleRateBox->addItem(QString::number(it), QVariant(it));
    }
    foreach (const int it, device.supportedChannelCounts()) {
            ui->channelBox->addItem(QString::number(it), QVariant(it));
    }
    foreach (const QString &it, device.supportedCodecs()) {
            ui->codecBox_2->addItem(it, QVariant(it));
    }

    foreach (const int it, device.supportedSampleSizes()) {
            ui->sampleSizeBox->addItem(QString::number(it), QVariant(it));
    }

    ui->sampleSizeBox->setCurrentIndex(1); //*******//
}

void Settings::displayOutputDeviceProperties(QAudioDeviceInfo device) {
    foreach (const int it, device.supportedSampleRates()) {
            ui->sampleRateBox_2->addItem(QString::number(it), QVariant(it));
    }
    foreach (const int it, device.supportedChannelCounts()) {
            ui->channelBox_2->addItem(QString::number(it), QVariant(it));
    }
    foreach (const QString &it, device.supportedCodecs()) {
            ui->codecBox_3->addItem(it, QVariant(it));
    }

    foreach (const int it, device.supportedSampleSizes()) {
            ui->sampleSizeBox_2->addItem(QString::number(it), QVariant(it));
    }

    ui->sampleSizeBox_2->setCurrentIndex(1); //*******//
}

QAudioFormat Settings::getSpeakerAudioFormat() {
    return formatSpeaker;
}

QAudioFormat Settings::getListennerAudioFormat() {
    return formatSpeaker;
}

QAudioDeviceInfo Settings::getInputDevice() {
    return selectedInputDevice;
}

QAudioDeviceInfo Settings::getOutputDevice() {
    return selectedOutputDevice;
}

QVariant Settings::boxValue(const QComboBox *box)
{
    int idx = box->currentIndex();
    if (idx == -1)
        return QVariant();

    return box->itemData(idx);
}

void Settings::applySettings() {
    formatSpeaker.setByteOrder(QAudioFormat::LittleEndian);
    formatSpeaker.setSampleType(QAudioFormat::UnSignedInt);

    formatSpeaker.setCodec(boxValue(ui->codecBox_2).toString());
    formatSpeaker.setSampleRate(boxValue(ui->sampleRateBox).toInt());
    formatSpeaker.setChannelCount(boxValue(ui->channelBox).toInt());
    formatSpeaker.setSampleSize(boxValue(ui->sampleSizeBox).toInt());

    formatListenner.setByteOrder(QAudioFormat::LittleEndian);
    formatListenner.setSampleType(QAudioFormat::UnSignedInt);

    formatListenner.setCodec(boxValue(ui->codecBox_3).toString());
    formatListenner.setSampleRate(boxValue(ui->sampleRateBox_2).toInt());
    formatListenner.setChannelCount(boxValue(ui->channelBox_2).toInt());
    formatListenner.setSampleSize(boxValue(ui->sampleSizeBox_2).toInt());
}

void Settings::cancelSetting() {
    this->close();
}
