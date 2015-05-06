#include "settings.h"
#include "ui_settings.h"

const QString Settings::appname_label = "WLAN_ConfSystem_Translator";
const QString Settings::organization_label = "Sapientia";
const QString Settings::server_address_label = "serveraddress";
const QString Settings::record_codec_label = "recordcodec";
const QString Settings::record_path_label = "recordpath";
const QString Settings::input_device_label = "inputdevice";
const QString Settings::output_device_label = "outputdevice";
const QString Settings::testmode_label = "testmode";
const QString Settings::log_label = "loglabel";
const QString Settings::logPath_label = "logpath";

const int Settings::CLIENT_TYPE = 3; //translator

Settings::Settings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Settings)
{
    ui->setupUi(this);

    settings = new QSettings(QSettings::IniFormat, QSettings::UserScope, organization_label, appname_label, this);

    //initialize selected properties for devices
    initSettingsValues();

    fileBrowser = new QFileDialog(this);

    connect(ui->applyButton, SIGNAL(clicked()), this, SLOT(applySettings()));
    connect(ui->cancelButton, SIGNAL(clicked()), this, SLOT(cancelSettings()));
    connect(ui->browserButton, SIGNAL(clicked()), this, SLOT(selectRecordPath()));
    connect(ui->logEnabled, SIGNAL(toggled(bool)), this, SLOT(enableLogPath(bool)));
    connect(ui->logPathBrowseBtn, SIGNAL(clicked()), this, SLOT(browseLogPath()));

}

Settings::~Settings()
{
    delete ui;
}

//initialize settings values in comboboxes
void Settings::initSettingsValues() {

    log = settings->value(log_label, false).toBool();
    if(log) {
        ui->logEnabled->setChecked(true);
        enableLogPath(log);
    }

    logPath = settings->value(logPath_label, QDir::currentPath()).toString();
    ui->logPath->setText(logPath);

    QString outputDeviceName = settings->value(output_device_label, "").toString();
    int index = 0;

    output_devices = QAudioDeviceInfo::availableDevices(QAudio::AudioOutput);
    foreach (const QAudioDeviceInfo it, output_devices) {
        ui->outputDeviceBox->addItem(it.deviceName(), QVariant(it.deviceName()));
        if(it.deviceName().compare(outputDeviceName) == 0) {
            setBoxIndex(ui->outputDeviceBox, index);
        }
        index++;
    }

    index = 0;

    QString inputDeviceName = settings->value(output_device_label, "").toString();
    input_devices = QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
    foreach (const QAudioDeviceInfo it, input_devices) {
        ui->inputDeviceBox->addItem(it.deviceName(), QVariant(it.deviceName()));
        if(it.deviceName().compare(inputDeviceName) == 0) {
            setBoxIndex(ui->inputDeviceBox, index);
        }
        index++;
    }

    //initialize recording codec
    index = 0;
    recordCodecs.append(QString("wav"));
    QString recordCodec = settings->value(record_codec_label, "wav").toString();
    foreach (const QString it, recordCodecs) {
        ui->codecBox->addItem(it, QVariant(it));
        if(it.compare(recordCodec) == 0) {
            setBoxIndex(ui->codecBox, index);
        }
        index++;
    }
    setBoxIndex(ui->codecBox, 0);

    //set record path
    recordPath = settings->value(record_path_label, QDir::currentPath()).toString();
    ui->displayPath->setText(recordPath);

    //testmode
    bool test = settings->value(testmode_label, false).toBool();
    if(test) {
        ui->testModeCheckBox->setChecked(true);
    }
    else {
        ui->testModeCheckBox->setChecked(false);
    }
}

//return the value held in combobox selectables
QVariant Settings::boxValue(const QComboBox *box)
{
    int idx = box->currentIndex();
    if (idx == -1)
        return QVariant();

    return box->itemData(idx);
}

//applying settings
void Settings::applySettings() {
    QString inputDeviceName = boxValue(ui->inputDeviceBox).toString();
    QString outputDeviceName = boxValue(ui->outputDeviceBox).toString();
    QString recordCodec = boxValue(ui->codecBox).toString();
    settings->setValue(input_device_label, inputDeviceName);
    settings->setValue(output_device_label, outputDeviceName);
    settings->setValue(record_path_label, recordPath);
    settings->setValue(record_codec_label, recordCodec);
    settings->setValue(testmode_label, ui->testModeCheckBox->isChecked());

    if(ui->logEnabled->isChecked() != log) {
        if(ui->logEnabled->isChecked()) {
            QString tempLogPath = ui->logPath->text();
            emit packetLogStarted(tempLogPath);
            settings->setValue(log_label, true);
            settings->setValue(logPath_label, tempLogPath);
        }
        else {
            emit packetLogStopped();
            settings->setValue(log_label, false);
        }
    }
    else {
        if(ui->logEnabled->isChecked()) {
            QString tmpPath = ui->logPath->text();
            if(tmpPath.compare(logPath) != 0) {
                emit packetLogStopped();
                emit packetLogStarted(tmpPath);
                settings->setValue(logPath_label, tmpPath);
            }
        }
    }
    this->close();
    cancelSettings();
}

void Settings::cancelSettings() {
    this->close();
}

//get index of a content from combobox
int Settings::getBoxIndex(QComboBox *box, QString *content) const {
    for(int i = 0; i < box->count(); ++i) {
        QString data = box->itemData(i, Qt::UserRole).toString();
        if(data.compare(*content) == 0) {
            return i;
        }
    }
    return -1;
}

//get index of a content from combobox
int Settings::getBoxIndex(QComboBox *box, int content) const {
    for(int i = 0; i < box->count(); ++i) {
        int data = box->itemData(i, Qt::UserRole).toInt();
        if(data == content) {
            return i;
        }
    }
    return -1;
}

void Settings::setBoxIndex(QComboBox *box, int index) {
    if(index > 0) {
        box->setCurrentIndex(index);
    }
    else {
        box->setCurrentIndex(0);
    }
}

void Settings::selectRecordPath() {
        recordPath = fileBrowser->getExistingDirectory(this, tr("Open Directory"));
        ui->displayPath->setText(recordPath);
}

void Settings::enableLogPath(bool checked) {
    if(checked) {
        ui->logPathBrowseBtn->setEnabled(true);
        ui->logPath->setEnabled(true);
    }
    else {
        ui->logPathBrowseBtn->setEnabled(false);
        ui->logPath->setEnabled(false);
    }
}

void Settings::browseLogPath() {
    QFileDialog *fileBrowser = new QFileDialog;
    fileBrowser->setAttribute(Qt::WA_DeleteOnClose);
    QString tmpLogPath = fileBrowser->getExistingDirectory(this, tr("Open Directory"));
    ui->logPath->setText(tmpLogPath);
}

bool Settings::checkIpAddress(const QString ip) {
    QHostAddress address(ip);
    if (QAbstractSocket::IPv4Protocol == address.protocol())
    {
       return true;
    }
    else
    {
       return false;
    }
}

