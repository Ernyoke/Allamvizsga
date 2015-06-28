#include "settings.h"
#include "ui_settings.h"

const QString Settings::appname_label = "WLAN_ConfSystem_Listener";
const QString Settings::organization_label = "Sapientia";
const QString Settings::server_address_label = "serveraddress";
const QString Settings::record_codec_label = "recordcodec";
const QString Settings::record_path_label = "recordpath";
const QString Settings::output_device_label = "outputdevice";

const int Settings::CLIENT_TYPE = 2; //listener client type

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

//    serverPort = 10000;
//    clientPort = 40000;

}

Settings::~Settings()
{
    qDebug() << "Settings deleted!";
    delete ui;
}

void Settings::initSettingsValues() {

    QString outputDeviceName = settings->value(output_device_label, "").toString();
    int index = 0;

    output_devices = QAudioDeviceInfo::availableDevices(QAudio::AudioOutput);
    foreach (const QAudioDeviceInfo it,output_devices) {
            ui->deviceBox->addItem(it.deviceName(), QVariant(it.deviceName()));
            if(it.deviceName().compare(outputDeviceName) == 0) {
                setBoxIndex(ui->deviceBox, index);
            }
            index++;
    }

    //initialize recording codec
    recordCodecs.append(QString("wav"));
    QString recordCodec = settings->value(record_codec_label, "wav").toString();
    ui->codecBox->addItem(recordCodec);
    setBoxIndex(ui->codecBox, 0);

    //set record path
    recordPath = settings->value(record_path_label, QDir::currentPath()).toString();
    ui->displayPath->setText(recordPath);
}

int Settings::getBoxIndex(QComboBox *box, QString *content) const {
    for(int i = 0; i < box->count(); ++i) {
        QString data = box->itemData(i, Qt::UserRole).toString();
        if(data.compare(*content) == 0) {
            return i;
        }
    }
    return -1;
}

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

QVariant Settings::boxValue(const QComboBox *box)
{
    int idx = box->currentIndex();
    if (idx == -1) {
        return QVariant();
    }
    return box->itemData(idx);
}

void Settings::applySettings() {
    QString deviceName = boxValue(ui->deviceBox).toString();
    settings->setValue(output_device_label, deviceName);
    settings->setValue(record_path_label, recordPath);
    cancelSettings();
}

void Settings::cancelSettings() {
    this->close();
}


void Settings::selectRecordPath() {
    recordPath = fileBrowser->getExistingDirectory();
    ui->displayPath->setText(recordPath);
}


bool Settings::checkIpAddress(const QString ip){
    QHostAddress address(ip);
    if (QAbstractSocket::IPv4Protocol == address.protocol())
    {
       qDebug("Valid IPv4 address.");
       return true;
    }
    else if (QAbstractSocket::IPv6Protocol == address.protocol())
    {
       qDebug("Valid IPv6 address.");
       return true;
    }
    else
    {
       qDebug("Unknown or invalid address.");
       return false;
    }
}

