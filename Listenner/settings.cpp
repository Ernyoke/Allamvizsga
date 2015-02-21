#include "settings.h"
#include "ui_settings.h"

const static QString settingsFile = "settings.xml";

Settings::Settings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Settings)
{
    ui->setupUi(this);

    output_devices = QAudioDeviceInfo::availableDevices(QAudio::AudioOutput);
    foreach (const QAudioDeviceInfo it,output_devices) {
            ui->deviceBox->addItem(it.deviceName(), QVariant(it.deviceName()));
    }

    //read settings from XML file into structs
    readSettingsFromXML();

    selectedDevice = QAudioDeviceInfo::defaultOutputDevice();
    for(QList<QAudioDeviceInfo>::iterator it = output_devices.begin(); it != output_devices.end(); ++it) {
        if(it->deviceName().compare(xml_outdev.dev_name) == 0) {
            selectedDevice = *it;
            break;
        }
    }

    //initialize selected properties for devices
    initSettingsValues();

    //initialize QAudioFormats for devices
    setFormatProperties();

    //initialize recording codec
    ui->codecBox->addItem("wav");
    recordCodec = WAV;

    //initialize recording path
    recordPath = QDir::currentPath();
    ui->displayPath->setText(recordPath);

    fileBrowser = new QFileDialog(this);

    connect(ui->applyButton, SIGNAL(clicked()), this, SLOT(applySettings()));
    connect(ui->cancelButton, SIGNAL(clicked()), this, SLOT(cancelSetting()));
    connect(ui->deviceBox, SIGNAL(activated(int)), this, SLOT(changeDevice(int)));
    connect(ui->browserButton, SIGNAL(clicked()), this, SLOT(selectRecordPath()));

    address = new QHostAddress("127.0.0.1");
    serverPort = 10000;
    clientPort = 40000;
    clientType = 2; //client type set to speaker
    clientId = 0; //set initial client id to 0

}

Settings::~Settings()
{
    qDebug() << "Settings deleted!";
    delete ui;
}

void Settings::initSettingsValues() {
    //outdev
    setBoxIndex(ui->deviceBox, getBoxIndex(ui->deviceBox, &xml_outdev.dev_name));
}


void Settings::changeDevice(int index) {
    QAudioDeviceInfo selectedDevice = output_devices.at(index);
}


//update format with given settings
void Settings::setFormatProperties() {
    xml_outdev.dev_name = selectedDevice.deviceName();
}


//read the settings from XML and parse them
//update the comboboxes
void Settings::readSettingsFromXML() {
    QFile s_file(settingsFile);
    if(s_file.open(QIODevice::ReadOnly)) {
        QXmlStreamReader XMLReader;
        XMLReader.setDevice(&s_file);
        while(!XMLReader.atEnd()) {
            XMLReader.readNextStartElement();
            if(XMLReader.isStartElement()) {
                if(XMLReader.name() == "audioformat") {
                    QXmlStreamAttributes atr = XMLReader.attributes();
                    if(atr.value("direction").toString().compare("output") == 0) {
                        while(XMLReader.readNextStartElement()) {
                            if(XMLReader.name() == "devicename") {
                                xml_outdev.dev_name = XMLReader.readElementText();
                            }
                            if(XMLReader.name() == "samplerate") {
                                xml_outdev.sample_rate = XMLReader.readElementText().toInt();
                            }
                            if(XMLReader.name() == "codec") {
                                xml_outdev.codec = XMLReader.readElementText();
                            }
                            if(XMLReader.name() == "channelcount") {
                                xml_outdev.channels = XMLReader.readElementText().toInt();
                            }
                            if(XMLReader.name() == "samplesize") {
                                xml_outdev.sample_size = XMLReader.readElementText().toInt();
                            }
                        }
                    }
                }
            }
        }
    }
}

int Settings::getBoxIndex(QComboBox *box, QString *content) {
    for(int i = 0; i < box->count(); ++i) {
        QString data = box->itemData(i, Qt::UserRole).toString();
        if(data.compare(*content) == 0) {
            return i;
        }
    }
    return -1;
}

int Settings::getBoxIndex(QComboBox *box, int content) {
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

QString Settings::getRecordPath() {
    return recordPath;
}

QAudioDeviceInfo Settings::getOutputDevice() {
    return selectedDevice;
}

QHostAddress* Settings::getServerAddress() {
    return address;
}

quint16 Settings::getServerPort() {
    return serverPort;
}

quint16 Settings::getClientPort() {
    return clientPort;
}

quint32 Settings::getClientType() {
    return clientType;
}

void Settings::setClientId(quint32 id) {
    this->clientId = id;
}

quint32 Settings::getClientId() {
    return clientId;
}

QVariant Settings::boxValue(const QComboBox *box)
{
    int idx = box->currentIndex();
    if (idx == -1)
        return QVariant();

    return box->itemData(idx);
}

void Settings::applySettings() {

    //update formats first
    setFormatProperties();

    recordPath = ui->displayPath->text();

    //store settings in output XML file
    QFile s_file(settingsFile);
    s_file.open(QIODevice::WriteOnly);
    QXmlStreamWriter xmlWriter(&s_file);
    xmlWriter.setAutoFormatting(true);
    xmlWriter.writeStartDocument();
    xmlWriter.writeStartElement("settings");
    xmlWriter.writeStartElement("audioformat");
    QXmlStreamAttribute atr("direction", "output");
    xmlWriter.writeAttribute(atr);
    xmlWriter.writeTextElement("devicename", boxValue(ui->deviceBox).toString());
    xmlWriter.writeEndElement();
    xmlWriter.writeEndElement();
    xmlWriter.writeEndDocument();
    s_file.close();

}

void Settings::cancelSetting() {
    this->close();
}

Settings::CODEC Settings::getRecordCodec() {
    return WAV;
}

void Settings::selectRecordPath() {
        recordPath = fileBrowser->getExistingDirectory();
        ui->displayPath->setText(recordPath);
}

bool Settings::setServerAddress(QString address) {
    if(checkIpAddress(address)) {
        this->address = new QHostAddress(address);
        return true;
    }
    return false;
}

bool Settings::checkIpAddress(QString ip) {
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

