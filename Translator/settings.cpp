#include "settings.h"
#include "ui_settings.h"

const static QString settingsFile = "settings.xml";

Settings::Settings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Settings)
{
    ui->setupUi(this);

    input_devices = QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
    foreach (const QAudioDeviceInfo it,input_devices) {
            ui->inputDeviceBox->addItem(it.deviceName(), QVariant(it.deviceName()));
    }

    output_devices = QAudioDeviceInfo::availableDevices(QAudio::AudioOutput);
    foreach (const QAudioDeviceInfo it,output_devices) {
            ui->outputDeviceBox->addItem(it.deviceName(), QVariant(it.deviceName()));
    }

    //read settings from XML file into structs
    readSettingsFromXML();

    //set the input and output device if it exists or set it default;
    activeInputDevice = QAudioDeviceInfo::defaultInputDevice();
    for(QList<QAudioDeviceInfo>::iterator it = input_devices.begin(); it != input_devices.end(); ++it) {
        if(it->deviceName().compare(inputDeviceName) == 0) {
            activeInputDevice = *it;
            break;
        }
    }

    activeOutputDevice = QAudioDeviceInfo::defaultOutputDevice();
    for(QList<QAudioDeviceInfo>::iterator it = output_devices.begin(); it != output_devices.end(); ++it) {
        if(it->deviceName().compare(outputDeviceName) == 0) {
            activeOutputDevice = *it;
            break;
        }
    }


    //initialize recording codec
    ui->codecBox->addItem("wav");
    recordCodec = WAV;

    //initialize recording path
    recordPath = QDir::currentPath();
    ui->displayPath->setText(recordPath);

    fileBrowser = new QFileDialog(this);

    connect(ui->applyButton, SIGNAL(clicked()), this, SLOT(applySettings()));
    connect(ui->cancelButton, SIGNAL(clicked()), this, SLOT(cancelSetting()));
    connect(ui->browserButton, SIGNAL(clicked()), this, SLOT(selectRecordPath()));
    connect(ui->inputDeviceBox, SIGNAL(activated(int)), this, SLOT(changeInputDevice(int)));
    connect(ui->outputDeviceBox, SIGNAL(activated(int)), this, SLOT(changeOutputDevice(int)));

    address = NULL;
    serverPort = 10000;
    clientPort = 40000;
    clientPortForSound = 20000;
    clientType = 3; //client type set to speaker
    clientId = 0; //set initial client id to 0

}

Settings::~Settings()
{
    delete ui;
}

//initialize settings values in comboboxes
void Settings::initSettingsValues() {
    setBoxIndex(ui->inputDeviceBox, getBoxIndex(ui->inputDeviceBox, &inputDeviceName));
    setBoxIndex(ui->outputDeviceBox, getBoxIndex(ui->outputDeviceBox, &outputDeviceName));
}

void Settings::changeInputDevice(int index) {
    selectedInputDevice = output_devices.at(index);
}

void Settings::changeOutputDevice(int index) {
    selectedOutputDevice = output_devices.at(index);
}



QAudioDeviceInfo Settings::getInputDevice() {
    return activeInputDevice;
}

QAudioDeviceInfo Settings::getOutputDevice() {
    return activeOutputDevice;
}


QString Settings::getRecordPath() {
    return recordPath;
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

quint16 Settings::getClientPortForSound() {
    return clientPortForSound;
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

    //store settings in output XML file
    QFile s_file(settingsFile);
    s_file.open(QIODevice::WriteOnly);
    QXmlStreamWriter xmlWriter(&s_file);
    xmlWriter.setAutoFormatting(true);
    xmlWriter.writeStartDocument();
    xmlWriter.writeStartElement("settings");

    //settings for outputdevice
    xmlWriter.writeStartElement("audioformat");
    QXmlStreamAttribute atr("direction", "output");
    xmlWriter.writeAttribute(atr);
    xmlWriter.writeTextElement("devicename", boxValue(ui->outputDeviceBox).toString());
    xmlWriter.writeEndElement();

    //settings for inputdevice
    xmlWriter.writeStartElement("audioformat");
    QXmlStreamAttribute atr2("direction", "input");
    xmlWriter.writeAttribute(atr2);
    xmlWriter.writeTextElement("devicename", boxValue(ui->inputDeviceBox).toString());
    xmlWriter.writeEndElement();
    xmlWriter.writeEndDocument();
    s_file.close();

    recordPath = ui->displayPath->text();
}

void Settings::cancelSetting() {
    this->close();
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
                    if(atr.value("direction").toString().compare("input") == 0) {
                        while(XMLReader.readNextStartElement()) {
                            if(XMLReader.name() == "devicename") {
                                inputDeviceName = XMLReader.readElementText();
                            }
                        }
                    }
                    else {
                        if(atr.value("direction").toString().compare("output") == 0) {
                            while(XMLReader.readNextStartElement()) {
                                if(XMLReader.name() == "devicename") {
                                    outputDeviceName = XMLReader.readElementText();
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

//get index of a content from combobox
int Settings::getBoxIndex(QComboBox *box, QString *content) {
    for(int i = 0; i < box->count(); ++i) {
        QString data = box->itemData(i, Qt::UserRole).toString();
        if(data.compare(*content) == 0) {
            return i;
        }
    }
    return -1;
}

//get index of a content from combobox
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

Settings::CODEC Settings::getRecordCodec() {
    return this->recordCodec;
}

void Settings::selectRecordPath() {
        recordPath = fileBrowser->getExistingDirectory(this, tr("Open Directory"));
        ui->displayPath->setText(recordPath);
}


//this event is overloaded for refreshing settings dialog on every showup
void Settings::showEvent(QShowEvent *event) {
    QWidget::showEvent(event);
    qDebug() << "show";

    initSettingsValues();
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

