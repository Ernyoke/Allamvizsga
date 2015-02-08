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
    selectedInputDevice = QAudioDeviceInfo::defaultInputDevice();
    selectedOutputDevice = QAudioDeviceInfo::defaultOutputDevice();
    for(QList<QAudioDeviceInfo>::iterator it = input_devices.begin(); it != input_devices.end(); ++it) {
        if(it->deviceName().compare(xml_indev.dev_name) == 0) {
            selectedInputDevice = *it;
            break;
        }
    }

    for(QList<QAudioDeviceInfo>::iterator it = output_devices.begin(); it != output_devices.end(); ++it) {
        if(it->deviceName().compare(xml_outdev.dev_name) == 0) {
            selectedOutputDevice = *it;
            break;
        }
    }

    //display all available properties for selected audio devices
    displayInputDeviceProperties(selectedInputDevice);
    displayOutputDeviceProperties(selectedOutputDevice);

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
    connect(ui->browserButton, SIGNAL(clicked()), this, SLOT(selectRecordPath()));
    connect(ui->inputDeviceBox, SIGNAL(activated(int)), this, SLOT(changeInputDevice(int)));
    connect(ui->outputDeviceBox, SIGNAL(activated(int)), this, SLOT(changeOutputDevice(int)));

    address = NULL;
    serverPort = 10000;
    clientPort = 40000;
    clientType = 3; //client type set to translator
    clientId = 0; //set initial client id to 0

}

Settings::~Settings()
{
    delete ui;
}

//display all properties in comboboxes for inputdevice
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

}

//display all properties in comboboxes for outputdevice
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

}

//initialize settings values in comboboxes
void Settings::initSettingsValues() {
    //inputdev
    setBoxIndex(ui->inputDeviceBox, getBoxIndex(ui->inputDeviceBox, &xml_indev.dev_name));
    setBoxIndex(ui->sampleRateBox, getBoxIndex(ui->sampleRateBox, xml_indev.sample_rate));
    setBoxIndex(ui->codecBox_2, getBoxIndex(ui->codecBox_2, &xml_indev.codec));
    setBoxIndex(ui->channelBox, getBoxIndex(ui->channelBox, xml_indev.channels));
    setBoxIndex(ui->sampleSizeBox, getBoxIndex(ui->sampleSizeBox, xml_indev.sample_size));

    //outdev
    setBoxIndex(ui->outputDeviceBox, getBoxIndex(ui->outputDeviceBox, &xml_outdev.dev_name));
    setBoxIndex(ui->sampleRateBox_2, getBoxIndex(ui->sampleRateBox_2, xml_outdev.sample_rate));
    setBoxIndex(ui->codecBox_3, getBoxIndex(ui->codecBox_3, &xml_outdev.codec));
    setBoxIndex(ui->channelBox_2, getBoxIndex(ui->channelBox_2, xml_outdev.channels));
    setBoxIndex(ui->sampleSizeBox_2, getBoxIndex(ui->sampleSizeBox_2, xml_outdev.sample_size));
}

QAudioFormat Settings::getSpeakerAudioFormat() {
    return formatSpeaker;
}

QAudioFormat Settings::getListennerAudioFormat() {
    return formatListenner;
}

QAudioDeviceInfo Settings::getInputDevice() {
    return selectedInputDevice;
}

QAudioDeviceInfo Settings::getOutputDevice() {
    return selectedOutputDevice;
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

void Settings::changeOutputDevice(int index) {
    QAudioDeviceInfo selectedDevice = output_devices.at(index);
    ui->sampleRateBox_2->clear();
    ui->channelBox_2->clear();
    ui->sampleSizeBox_2->clear();
    ui->codecBox_3->clear();
    displayOutputDeviceProperties(selectedDevice);
}

void Settings::changeInputDevice(int index) {
    QAudioDeviceInfo selectedDevice = input_devices.at(index);
    ui->sampleRateBox->clear();
    ui->channelBox->clear();
    ui->sampleSizeBox->clear();
    ui->codecBox_2->clear();
    displayInputDeviceProperties(selectedDevice);
}

//update format with given settings
void Settings::setFormatProperties() {
    formatSpeaker.setByteOrder(QAudioFormat::LittleEndian);
    formatSpeaker.setSampleType(QAudioFormat::UnSignedInt);

    xml_indev.dev_name = selectedInputDevice.deviceName();
    xml_indev.codec = boxValue(ui->codecBox_2).toString();
    xml_indev.channels = boxValue(ui->channelBox).toInt();
    xml_indev.sample_rate = boxValue(ui->sampleRateBox).toInt();
    xml_indev.sample_size = boxValue(ui->sampleSizeBox).toInt();

    formatSpeaker.setCodec(xml_indev.codec);
    formatSpeaker.setSampleRate(xml_indev.sample_rate);
    formatSpeaker.setChannelCount(xml_indev.channels);
    formatSpeaker.setSampleSize(xml_indev.sample_size);

    formatListenner.setByteOrder(QAudioFormat::LittleEndian);
    formatListenner.setSampleType(QAudioFormat::UnSignedInt);

    xml_outdev.dev_name = selectedInputDevice.deviceName();
    xml_outdev.codec = boxValue(ui->codecBox_3).toString();
    xml_outdev.channels = boxValue(ui->channelBox_2).toInt();
    xml_outdev.sample_rate = boxValue(ui->sampleRateBox_2).toInt();
    xml_outdev.sample_size = boxValue(ui->sampleSizeBox_2).toInt();

    formatListenner.setCodec(xml_outdev.codec);
    formatListenner.setSampleRate(xml_outdev.sample_rate);
    formatListenner.setChannelCount(xml_outdev.channels);
    formatListenner.setSampleSize(xml_outdev.sample_size);
}

//applying settings
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

    //settings for outputdevice
    xmlWriter.writeStartElement("audioformat");
    QXmlStreamAttribute atr("direction", "output");
    xmlWriter.writeAttribute(atr);
    xmlWriter.writeTextElement("devicename", boxValue(ui->outputDeviceBox).toString());
    xmlWriter.writeTextElement("codec", boxValue(ui->codecBox_3).toString());
    xmlWriter.writeTextElement("samplerate", boxValue(ui->sampleRateBox_2).toString());
    xmlWriter.writeTextElement("channelcount", boxValue(ui->channelBox_2).toString());
    xmlWriter.writeTextElement("samplesize", boxValue(ui->sampleSizeBox_2).toString());
    xmlWriter.writeEndElement();

    //settings for inputdevice
    xmlWriter.writeStartElement("audioformat");
    QXmlStreamAttribute atr2("direction", "input");
    xmlWriter.writeAttribute(atr2);
    xmlWriter.writeTextElement("devicename", boxValue(ui->inputDeviceBox).toString());
    xmlWriter.writeTextElement("codec", boxValue(ui->codecBox_2).toString());
    xmlWriter.writeTextElement("samplerate", boxValue(ui->sampleRateBox).toString());
    xmlWriter.writeTextElement("channelcount", boxValue(ui->channelBox).toString());
    xmlWriter.writeTextElement("samplesize", boxValue(ui->sampleSizeBox).toString());
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
                    else {
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

