#include "settings.h"
#include "ui_settings.h"

Settings::Settings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Settings)
{
    ui->setupUi(this);

    selectedDevice = QAudioDeviceInfo::defaultOutputDevice();
    displayDeviceProperties(selectedDevice);

    output_devices = QAudioDeviceInfo::availableDevices(QAudio::AudioOutput);
    foreach (const QAudioDeviceInfo it,output_devices) {
            ui->deviceBox->addItem(it.deviceName(), QVariant(it.deviceName()));
    }

    applySettings();
    connect(ui->applyButton, SIGNAL(clicked()), this, SLOT(applySettings()));
    connect(ui->cancelButton, SIGNAL(clicked()), this, SLOT(cancelSetting()));
    connect(ui->deviceBox, SIGNAL(activated(int)), this, SLOT(changeDevice(int)));

}

Settings::~Settings()
{
    delete ui;


}

void Settings::changeDevice(int index) {
    QAudioDeviceInfo selectedDevice = output_devices.at(index);
    ui->sampleRateBox->clear();
    ui->channelBox->clear();
    ui->sampleSizeBox->clear();
    ui->codecBox_2->clear();
    displayDeviceProperties(selectedDevice);
}

void Settings::displayDeviceProperties(QAudioDeviceInfo device) {
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

QAudioFormat Settings::getListennerAudioFormat() {
    return formatListenner;
}

QAudioDeviceInfo Settings::getOutputDevice() {
    return selectedDevice;
}

QVariant Settings::boxValue(const QComboBox *box)
{
    int idx = box->currentIndex();
    if (idx == -1)
        return QVariant();

    return box->itemData(idx);
}

void Settings::applySettings() {
    formatListenner.setByteOrder(QAudioFormat::LittleEndian);
    formatListenner.setSampleType(QAudioFormat::UnSignedInt);

    formatListenner.setCodec(boxValue(ui->codecBox_2).toString());
    formatListenner.setSampleRate(boxValue(ui->sampleRateBox).toInt());
    formatListenner.setChannelCount(boxValue(ui->channelBox).toInt());
    formatListenner.setSampleSize(boxValue(ui->sampleSizeBox).toInt());
}

void Settings::cancelSetting() {
    this->close();
}
