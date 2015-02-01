#include "settings.h"
#include "ui_settings.h"

const static QString settingsFile = "settings.xml";

Settings::Settings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Settings)
{
    ui->setupUi(this);

    displayDeviceProperties(selectedDevice);

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

    displayDeviceProperties(selectedDevice);

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
    setBoxIndex(ui->sampleRateBox, getBoxIndex(ui->sampleRateBox, xml_outdev.sample_rate));
    setBoxIndex(ui->codecBox_2, getBoxIndex(ui->codecBox_2, &xml_outdev.codec));
    setBoxIndex(ui->channelBox, getBoxIndex(ui->channelBox, xml_outdev.channels));
    setBoxIndex(ui->sampleSizeBox, getBoxIndex(ui->sampleSizeBox, xml_outdev.sample_size));
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
    ui->sampleRateBox->setEditable(false);
    foreach (const int it, device.supportedChannelCounts()) {
            ui->channelBox->addItem(QString::number(it), QVariant(it));
    }
    ui->channelBox->setEditable(false);
    foreach (const QString &it, device.supportedCodecs()) {
            ui->codecBox_2->addItem(it, QVariant(it));
    }
    ui->codecBox_2->setEditable(false);
    foreach (const int it, device.supportedSampleSizes()) {
            ui->sampleSizeBox->addItem(QString::number(it), QVariant(it));
    }
    ui->sampleSizeBox->setEditable(false);
}

//update format with given settings
void Settings::setFormatProperties() {
    formatListenner.setByteOrder(QAudioFormat::LittleEndian);
    formatListenner.setSampleType(QAudioFormat::UnSignedInt);

    xml_outdev.dev_name = selectedDevice.deviceName();
    xml_outdev.codec = boxValue(ui->codecBox_2).toString();
    xml_outdev.channels = boxValue(ui->channelBox).toInt();
    xml_outdev.sample_rate = boxValue(ui->sampleRateBox).toInt();
    xml_outdev.sample_size = boxValue(ui->sampleSizeBox).toInt();

    formatListenner.setCodec(xml_outdev.codec);
    formatListenner.setSampleRate(xml_outdev.sample_rate);
    formatListenner.setChannelCount(xml_outdev.channels);
    formatListenner.setSampleSize(xml_outdev.sample_size);
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

QAudioFormat Settings::getListennerAudioFormat() {
    return formatListenner;
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

void Settings::applySettings() {

    //update formats first
    setFormatProperties();

    formatListenner.setByteOrder(QAudioFormat::LittleEndian);
    formatListenner.setSampleType(QAudioFormat::UnSignedInt);

    formatListenner.setCodec(boxValue(ui->codecBox_2).toString());
    formatListenner.setSampleRate(boxValue(ui->sampleRateBox).toInt());
    formatListenner.setChannelCount(boxValue(ui->channelBox).toInt());
    formatListenner.setSampleSize(boxValue(ui->sampleSizeBox).toInt());

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

Settings::CODEC Settings::getRecordCodec() {
    return WAV;
}

void Settings::selectRecordPath() {
        recordPath = fileBrowser->getExistingDirectory();
        ui->displayPath->setText(recordPath);
}

