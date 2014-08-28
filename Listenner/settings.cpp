#include "settings.h"
#include "ui_settings.h"

const static QString settingsFile = "settings.xml";

Settings::Settings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Settings)
{
    ui->setupUi(this);

    selectedDevice = QAudioDeviceInfo::defaultOutputDevice();
    displayDeviceProperties(selectedDevice);

    output_devices = QAudioDeviceInfo::availableDevices(QAudio::AudioOutput);
    foreach (const QAudioDeviceInfo it,output_devices) {
            ui->deviceBox->addItem(it.deviceName(), QVariant(it.deviceName()));
    }

    ui->codecBox->addItem("wav");
    recordCodec = WAV;

    fileBrowser = new QFileDialog(this);

    applySettings();
    connect(ui->applyButton, SIGNAL(clicked()), this, SLOT(applySettings()));
    connect(ui->cancelButton, SIGNAL(clicked()), this, SLOT(cancelSetting()));
    connect(ui->deviceBox, SIGNAL(activated(int)), this, SLOT(changeDevice(int)));
    connect(ui->browserButton, SIGNAL(clicked()), this, SLOT(selectRecordPath()));

}

Settings::~Settings()
{
    qDebug() << "Settings deleted!";
    delete ui;
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
    readSettingsFromXML();
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
                                QString content = XMLReader.readElementText();
                                setBoxIndex(ui->deviceBox, getBoxIndex(ui->deviceBox, &content));
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

QVariant Settings::boxValue(const QComboBox *box)
{
    int idx = box->currentIndex();
    if (idx == -1)
        return QVariant();

    return box->itemData(idx);
}

void Settings::applySettings() {
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

