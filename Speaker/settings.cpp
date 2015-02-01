#include "settings.h"
#include "ui_settings.h"

const static QString settingsFile = "settings.xml";

Settings::Settings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Settings)
{
    ui->setupUi(this);

    selectedDevice = QAudioDeviceInfo::defaultInputDevice();
    displayDeviceProperties(selectedDevice);

    input_devices = QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
    foreach (const QAudioDeviceInfo it,input_devices) {
            ui->deviceBox->addItem(it.deviceName(), QVariant(it.deviceName()));
    }

    //read settings from XML file into structs
    readSettingsFromXML();

    //set the input and output device if it exists or set it default;
    selectedDevice = QAudioDeviceInfo::defaultInputDevice();
    for(QList<QAudioDeviceInfo>::iterator it = input_devices.begin(); it != input_devices.end(); ++it) {
        if(it->deviceName().compare(xml_indev.dev_name) == 0) {
            selectedDevice = *it;
            break;
        }
    }

    //display all available properties for selected audio devices
    displayDeviceProperties(selectedDevice);

    //initialize selected properties for devices
    initSettingsValues();

    //initialize QAudioFormats for devices
    setFormatProperties();

    connect(ui->applyButton, SIGNAL(clicked()), this, SLOT(applySettings()));
    connect(ui->cancelButton, SIGNAL(clicked()), this, SLOT(cancelSetting()));
    connect(ui->deviceBox, SIGNAL(activated(int)), this, SLOT(changeDevice(int)));

    address = new QHostAddress("127.0.0.1");
    serverPort = 10000;
    clientPort = 40000;
    clientType = 1; //client type set to speaker
    clientId = 0; //set initial client id to 0

}

Settings::~Settings()
{
    delete ui;
    qDebug() << "Settings destruct!";
}

QAudioFormat Settings::getSpeakerAudioFormat() {
    return formatSpeaker;
}

QAudioDeviceInfo Settings::getInputDevice() {
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

quint16 Settings::getClientType() {
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

//initialize settings values in comboboxes
void Settings::initSettingsValues() {
    //inputdev
    setBoxIndex(ui->deviceBox, getBoxIndex(ui->deviceBox, &xml_indev.dev_name));
    setBoxIndex(ui->sampleRateBox, getBoxIndex(ui->sampleRateBox, xml_indev.sample_rate));
    setBoxIndex(ui->codecBox_2, getBoxIndex(ui->codecBox_2, &xml_indev.codec));
    setBoxIndex(ui->channelBox, getBoxIndex(ui->channelBox, xml_indev.channels));
    setBoxIndex(ui->sampleSizeBox, getBoxIndex(ui->sampleSizeBox, xml_indev.sample_size));
}

void Settings::changeDevice(int index) {
    QAudioDeviceInfo selectedDevice = input_devices.at(index);
    ui->sampleRateBox->clear();
    ui->channelBox->clear();
    ui->sampleSizeBox->clear();
    ui->codecBox_2->clear();
    displayDeviceProperties(selectedDevice);
}

//display all properties in comboboxes for inputdevice
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
    readSettingsFromXML();
}

void Settings::setFormatProperties() {
    formatSpeaker.setByteOrder(QAudioFormat::LittleEndian);
    formatSpeaker.setSampleType(QAudioFormat::UnSignedInt);

    xml_indev.dev_name = selectedDevice.deviceName();
    xml_indev.codec = boxValue(ui->codecBox_2).toString();
    xml_indev.channels = boxValue(ui->channelBox).toInt();
    xml_indev.sample_rate = boxValue(ui->sampleRateBox).toInt();
    xml_indev.sample_size = boxValue(ui->sampleSizeBox).toInt();

    formatSpeaker.setCodec(xml_indev.codec);
    formatSpeaker.setSampleRate(xml_indev.sample_rate);
    formatSpeaker.setChannelCount(xml_indev.channels);
    formatSpeaker.setSampleSize(xml_indev.sample_size);
}

void Settings::applySettings() {
    //update formats first
    setFormatProperties();

    //store settings in output XML file
    QFile s_file(settingsFile);
    s_file.open(QIODevice::WriteOnly);
    QXmlStreamWriter xmlWriter(&s_file);
    xmlWriter.setAutoFormatting(true);
    xmlWriter.writeStartDocument();
    xmlWriter.writeStartElement("settings");
    xmlWriter.writeStartElement("audioformat");
    QXmlStreamAttribute atr("direction", "input");
    xmlWriter.writeAttribute(atr);
    xmlWriter.writeTextElement("devicename", boxValue(ui->deviceBox).toString());
    xmlWriter.writeTextElement("codec", boxValue(ui->codecBox_2).toString());
    xmlWriter.writeTextElement("samplerate", boxValue(ui->sampleRateBox).toString());
    xmlWriter.writeTextElement("channelcount", boxValue(ui->channelBox).toString());
    xmlWriter.writeTextElement("samplesize", boxValue(ui->sampleSizeBox).toString());
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
                                xml_indev.dev_name = XMLReader.readElementText();
                            }
                            if(XMLReader.name() == "samplerate") {
                                xml_indev.sample_rate = XMLReader.readElementText().toInt();
                            }
                            if(XMLReader.name() == "codec") {
                                xml_indev.codec = XMLReader.readElementText();
                            }
                            if(XMLReader.name() == "channelcount") {
                                xml_indev.channels = XMLReader.readElementText().toInt();
                            }
                            if(XMLReader.name() == "samplesize") {
                                xml_indev.sample_size = XMLReader.readElementText().toInt();
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





