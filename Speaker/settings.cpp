#include "settings.h"
#include "ui_settings.h"

const static QString settingsFileName = "settings.xml";

Settings::Settings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Settings)
{
    ui->setupUi(this);

    selectedInputDevice = QAudioDeviceInfo::defaultInputDevice();
    activeInputDevice = selectedInputDevice;

    input_devices = QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
    inputDeviceCounter = input_devices.size();
    foreach (const QAudioDeviceInfo it,input_devices) {
            ui->deviceBox->addItem(it.deviceName(), QVariant(it.deviceName()));
    }

    QFile settingsFile(settingsFileName);
    if(!settingsFile.exists()) {
        if(inputDeviceName.isEmpty()) {
            inputDeviceName = activeInputDevice.deviceName();
        }
        applySettings();
    }
    else {
        readSettingsFromXML();

        //set the input and output device if it exists or set it default;
        for(QList<QAudioDeviceInfo>::iterator it = input_devices.begin(); it != input_devices.end(); ++it) {
            if(it->deviceName().compare(inputDeviceName) == 0) {
                selectedInputDevice = *it;
                activeInputDevice = *it;
                break;
            }
        }

    }

    //initialize selected properties for devices
    initSettingsValues();

    connect(ui->applyButton, SIGNAL(clicked()), this, SLOT(applySettings()));
    connect(ui->cancelButton, SIGNAL(clicked()), this, SLOT(cancelSetting()));
    connect(ui->deviceBox, SIGNAL(activated(int)), this, SLOT(changeDevice(int)));

    address = NULL;
    serverPort = 10000;
    clientPort = 40000;
    clientPortForSound = 20000;
    clientType = 1; //client type set to speaker
    clientId = 0; //set initial client id to 0

}

Settings::~Settings()
{
    delete ui;
    delete address;
    qDebug() << "Settings destruct!";
}

QAudioDeviceInfo Settings::getInputDevice() const {
    if(inputDeviceCounter < 1) {
        NoAudioDeviceException *exception = new NoAudioDeviceException;
        exception->setMessage("No input audiodevice found!");
    }
    else {
        return activeInputDevice;
    }
}


QHostAddress Settings::getServerAddress() const {
    return *address;
}


quint16 Settings::getServerPort() const{
    return serverPort;
}

void Settings::setClientPort(const qint32 clientPort) {
    this->clientPort = clientPort;
}

quint16 Settings::getClientPort() const {
    return clientPort;
}

quint16 Settings::getClientPortForSound() const {
    return clientPortForSound;
}

quint32 Settings::getClientType() const {
    return clientType;
}

void Settings::setClientId(const quint32 id) {
    this->clientId = id;
}

quint32 Settings::getClientId() const {
    return clientId;
}


QVariant Settings::boxValue(const QComboBox *box)
{
    int idx = box->currentIndex();
    if (idx == -1)
        return QVariant();

    return box->itemData(idx);
}

//initialize settings values in comboboxes
void Settings::initSettingsValues() {
    setBoxIndex(ui->deviceBox, getBoxIndex(ui->deviceBox, &inputDeviceName));
}

void Settings::changeDevice(int index) {
    selectedInputDevice = input_devices.at(index);
}

void Settings::applySettings() {
    //update formats first
    activeInputDevice = selectedInputDevice;

    //store settings in output XML file
    QFile s_file(settingsFileName);
    s_file.open(QIODevice::WriteOnly);
    QXmlStreamWriter xmlWriter(&s_file);
    xmlWriter.setAutoFormatting(true);
    xmlWriter.writeStartDocument();
    xmlWriter.writeStartElement("settings");
    xmlWriter.writeStartElement("audiodevice");
    QXmlStreamAttribute atr("direction", "input");
    xmlWriter.writeAttribute(atr);
    xmlWriter.writeTextElement("devicename", selectedInputDevice.deviceName());
    xmlWriter.writeEndElement();
    xmlWriter.writeEndElement();
    xmlWriter.writeEndDocument();
    s_file.close();

}

void Settings::cancelSetting() {
    this->close();
}

//read the settings from XML and parse them
//update the comboboxes
void Settings::readSettingsFromXML() {
    QFile s_file(settingsFileName);
    if(s_file.open(QIODevice::ReadOnly)) {
        QXmlStreamReader XMLReader;
        XMLReader.setDevice(&s_file);
        while(!XMLReader.atEnd()) {
            XMLReader.readNextStartElement();
            if(XMLReader.isStartElement()) {
                if(XMLReader.name() == "audiodevice") {
                    QXmlStreamAttributes atr = XMLReader.attributes();
                    if(atr.value("direction").toString().compare("input") == 0) {
                        while(XMLReader.readNextStartElement()) {
                            if(XMLReader.name() == "devicename") {
                                inputDeviceName = XMLReader.readElementText();
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

bool Settings::setServerAddress(QString address) {
    if(checkIpAddress(address)) {
        this->address = new QHostAddress(address);
        return true;
    }
    return false;
}

bool Settings::checkIpAddress(QString& ip) {
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

bool Settings::testMode() const {
    return ui->testModeCheckBox->isChecked();
}





