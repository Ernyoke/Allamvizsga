#include "settings.h"
#include "ui_settings.h"

const static QString settingsFileName = "settings.xml";

Settings::Settings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Settings)
{
    ui->setupUi(this);

    output_devices = QAudioDeviceInfo::availableDevices(QAudio::AudioOutput);
    foreach (const QAudioDeviceInfo it,output_devices) {
            ui->deviceBox->addItem(it.deviceName(), QVariant(it.deviceName()));
    }

    QFile settingsFile(settingsFileName);

    if(settingsFile.exists()) {
        //read settings from XML file into structs
        readSettingsFromXML();

        activeOutputDevice = QAudioDeviceInfo::defaultOutputDevice();
        for(QList<QAudioDeviceInfo>::iterator it = output_devices.begin(); it != output_devices.end(); ++it) {
            if(it->deviceName().compare(outputDeviceName) == 0) {
                activeOutputDevice = *it;
                break;
            }
        }
    }
    else {
        if(outputDeviceName.isEmpty()) {
            outputDeviceName = activeOutputDevice.deviceName();
        }
        applySettings();
    }

    //initialize selected properties for devices
    initSettingsValues();


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
    setBoxIndex(ui->deviceBox, getBoxIndex(ui->deviceBox, &outputDeviceName));
}


void Settings::changeDevice(int index) {
    selectedOutputDevice = output_devices.at(index);
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
                if(XMLReader.name() == "audioformat") {
                    QXmlStreamAttributes atr = XMLReader.attributes();
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

QString Settings::getRecordPath() const {
    return recordPath;
}

QAudioDeviceInfo Settings::getOutputDevice() const {
    if(output_devices.size() < 1) {
        NoAudioDeviceException *exception = new NoAudioDeviceException;
        exception->setMessage("No output audiodevice found!");
        throw exception;
    }
    else {
        return activeOutputDevice;
    }
}

QHostAddress Settings::getServerAddress() const {
    return *address;
}

qint32 Settings::getServerPort() const {
    return serverPort;
}

void Settings::setClientPort(const qint32 port) {
    this->clientPort = port;
}

qint32 Settings::getClientPort() const {
    return clientPort;
}

qint32 Settings::getClientType() const {
    return clientType;
}

void Settings::setClientId(const qint32 id) {
    this->clientId = id;
}

qint32 Settings::getClientId() const {
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

    activeOutputDevice = selectedOutputDevice;
    recordPath = ui->displayPath->text();

    //store settings in output XML file
    QFile s_file(settingsFileName);
    s_file.open(QIODevice::WriteOnly);
    QXmlStreamWriter xmlWriter(&s_file);
    xmlWriter.setAutoFormatting(true);
    xmlWriter.writeStartDocument();
    xmlWriter.writeStartElement("settings");
    xmlWriter.writeStartElement("audioformat");
    QXmlStreamAttribute atr("direction", "output");
    xmlWriter.writeAttribute(atr);
    xmlWriter.writeTextElement("devicename", selectedOutputDevice.deviceName());
    xmlWriter.writeEndElement();
    xmlWriter.writeEndElement();
    xmlWriter.writeEndDocument();
    s_file.close();

}

void Settings::cancelSetting() {
    this->close();
}

Settings::CODEC Settings::getRecordCodec() const {
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

