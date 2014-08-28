#include "settings.h"
#include "ui_settings.h"

const static QString settingsFile = "settings.xml";

Settings::Settings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Settings)
{
    ui->setupUi(this);

    selectedInputDevice = QAudioDeviceInfo::defaultInputDevice();
    selectedOutputDevice = QAudioDeviceInfo::defaultOutputDevice();

    input_devices = QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
    foreach (const QAudioDeviceInfo it,input_devices) {
            ui->inputDeviceBox->addItem(it.deviceName(), QVariant(it.deviceName()));
    }

    output_devices = QAudioDeviceInfo::availableDevices(QAudio::AudioOutput);
    foreach (const QAudioDeviceInfo it,output_devices) {
            ui->outputDeviceBox->addItem(it.deviceName(), QVariant(it.deviceName()));
    }

    ui->codecBox->addItem("wav");
    recordCodec = WAV;

    fileBrowser = new QFileDialog(this);

    displayInputDeviceProperties(selectedInputDevice);
    displayOutputDeviceProperties(selectedOutputDevice);
    readSettingsFromXML();

    applySettings();
    connect(ui->applyButton, SIGNAL(clicked()), this, SLOT(applySettings()));
    connect(ui->cancelButton, SIGNAL(clicked()), this, SLOT(cancelSetting()));
    connect(ui->browserButton, SIGNAL(clicked()), this, SLOT(selectRecordPath()));
    connect(ui->inputDeviceBox, SIGNAL(activated(int)), this, SLOT(changeInputDevice(int)));
    connect(ui->outputDeviceBox, SIGNAL(activated(int)), this, SLOT(changeOutputDevice(int)));

}

Settings::~Settings()
{
    delete ui;
}

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

QAudioFormat Settings::getSpeakerAudioFormat() {
    return formatSpeaker;
}

QAudioFormat Settings::getListennerAudioFormat() {
    return formatSpeaker;
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
    displayInputDeviceProperties(selectedDevice);
}

void Settings::changeInputDevice(int index) {
    QAudioDeviceInfo selectedDevice = input_devices.at(index);
    ui->sampleRateBox->clear();
    ui->channelBox->clear();
    ui->sampleSizeBox->clear();
    ui->codecBox_2->clear();
    displayOutputDeviceProperties(selectedDevice);
}

void Settings::applySettings() {
    formatSpeaker.setByteOrder(QAudioFormat::LittleEndian);
    formatSpeaker.setSampleType(QAudioFormat::UnSignedInt);

    formatSpeaker.setCodec(boxValue(ui->codecBox_2).toString());
    formatSpeaker.setSampleRate(boxValue(ui->sampleRateBox).toInt());
    formatSpeaker.setChannelCount(boxValue(ui->channelBox).toInt());
    formatSpeaker.setSampleSize(boxValue(ui->sampleSizeBox).toInt());

    formatListenner.setByteOrder(QAudioFormat::LittleEndian);
    formatListenner.setSampleType(QAudioFormat::UnSignedInt);

    formatListenner.setCodec(boxValue(ui->codecBox_3).toString());
    formatListenner.setSampleRate(boxValue(ui->sampleRateBox_2).toInt());
    formatListenner.setChannelCount(boxValue(ui->channelBox_2).toInt());
    formatListenner.setSampleSize(boxValue(ui->sampleSizeBox_2).toInt());

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
                                QString content = XMLReader.readElementText();
                                setBoxIndex(ui->inputDeviceBox, getBoxIndex(ui->inputDeviceBox, &content));
                            }
                            if(XMLReader.name() == "samplerate") {
                                QString content = XMLReader.readElementText();
                                setBoxIndex(ui->sampleRateBox, getBoxIndex(ui->sampleRateBox, content.toInt()));
                            }
                            if(XMLReader.name() == "codec") {
                                QString content = XMLReader.readElementText();
                                setBoxIndex(ui->codecBox_2, getBoxIndex(ui->codecBox_2, &content));
                            }
                            if(XMLReader.name() == "channelcount") {
                                QString content = XMLReader.readElementText();
                                setBoxIndex(ui->channelBox, getBoxIndex(ui->channelBox, content.toInt()));
                            }
                            if(XMLReader.name() == "samplesize") {
                                QString content = XMLReader.readElementText();
                                setBoxIndex(ui->sampleSizeBox, getBoxIndex(ui->sampleSizeBox, content.toInt()));
                            }
                        }
                    }
                    else {
                        if(atr.value("direction").toString().compare("output") == 0) {
                            while(XMLReader.readNextStartElement()) {
                                if(XMLReader.name() == "devicename") {
                                    QString content = XMLReader.readElementText();
                                    setBoxIndex(ui->outputDeviceBox, getBoxIndex(ui->outputDeviceBox, &content));
                                }
                                if(XMLReader.name() == "samplerate") {
                                    QString content = XMLReader.readElementText();
                                    setBoxIndex(ui->sampleRateBox_2, getBoxIndex(ui->sampleRateBox_2, content.toInt()));
                                }
                                if(XMLReader.name() == "codec") {
                                    QString content = XMLReader.readElementText();
                                    setBoxIndex(ui->codecBox_3, getBoxIndex(ui->codecBox_3, &content));
                                }
                                if(XMLReader.name() == "channelcount") {
                                    QString content = XMLReader.readElementText();
                                    setBoxIndex(ui->channelBox_2, getBoxIndex(ui->channelBox_2, content.toInt()));
                                }
                                if(XMLReader.name() == "samplesize") {
                                    QString content = XMLReader.readElementText();
                                    setBoxIndex(ui->sampleSizeBox_2, getBoxIndex(ui->sampleSizeBox_2, content.toInt()));
                                }
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

Settings::CODEC Settings::getRecordCodec() {
    return WAV;
}

void Settings::selectRecordPath() {
        recordPath = fileBrowser->getExistingDirectory();
        ui->displayPath->setText(recordPath);
}
