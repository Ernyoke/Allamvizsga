#include "settings.h"
#include "ui_settings.h"

const QString Settings::appName_label = "WLAN_ConfSystem_Server";
const QString Settings::organization_label = "Sapientia";
const QString Settings::bcast_label = "broadcastaddress";
const QString Settings::log_label = "packetlog";
const QString Settings::logPath_label = "logPath";

Settings::Settings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Settings)
{
    ui->setupUi(this);

    settings = new QSettings(QSettings::IniFormat, QSettings::UserScope, organization_label, appName_label, this);
    readSettings();

    connect(ui->applyBtn, SIGNAL(clicked()), this, SLOT(saveSettings()));
    connect(ui->cancelBtn, SIGNAL(clicked()), this, SLOT(close()));
    connect(ui->browseBtn, SIGNAL(clicked()), this, SLOT(browse()));
}

Settings::~Settings()
{
    delete ui;
}

void Settings::readSettings() {
    broadcastAddress = settings->value(bcast_label, "192.168.0.255").toString();
    log = settings->value(log_label, false).toBool();
    logPath = settings->value(logPath_label, QDir::currentPath()).toString();
    ui->bcastAddr->setText(broadcastAddress);
    ui->logPath->setText(logPath);
    ui->logEnabled->setChecked(log);
}

void Settings::saveSettings() {
    QString tempBroadcastAddress = ui->bcastAddr->text();
    if(checkIpAddress(tempBroadcastAddress)) {
        if(tempBroadcastAddress.compare(broadcastAddress) != 0) {
            emit broadcastAddressChanged(tempBroadcastAddress);
            settings->setValue(bcast_label, QVariant(tempBroadcastAddress));
        }
    }
    else {
        showErrorMessage(QString("Invalid IP address!"));
        return;
    }
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
}

bool Settings::checkIpAddress(QString& ip) {
    QHostAddress address(ip);
    if (QAbstractSocket::IPv4Protocol == address.protocol())
    {
       qDebug("Valid IPv4 address.");
       return true;
    }
    else
    {
       qDebug("Unknown or invalid address.");
       return false;
    }
}

void Settings::showErrorMessage(QString message) {
    //
}

void Settings::browse() {
    QFileDialog fileBrowser;
    QString path = fileBrowser.getExistingDirectory();
    ui->logPath->setText(path);
}
