#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>
#include <QAudioDeviceInfo>
#include <QComboBox>
#include <QFileDialog>
#include <QDebug>
#include <QFile>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QXmlStreamAttribute>

namespace Ui {
class Settings;
}

class Settings : public QDialog
{
    Q_OBJECT

public:

    enum CODEC {WAV};

    explicit Settings(QWidget *parent = 0);
    ~Settings();

    QAudioFormat getSpeakerAudioFormat();
    QAudioFormat getListennerAudioFormat();
    QAudioDeviceInfo getInputDevice();
    QAudioDeviceInfo getOutputDevice();
    CODEC getRecordCodec();
    QString getRecordPath();

private:

    struct devinfo {
        QString dev_name;
        QString codec;
        int sample_rate;
        int channels;
        int sample_size;
    };

    devinfo xml_indev;
    devinfo xml_outdev;

    Ui::Settings *ui;
    QAudioFormat formatSpeaker;
    QAudioFormat formatListenner;
    QAudioDeviceInfo selectedInputDevice;
    QAudioDeviceInfo selectedOutputDevice;
    QList<QAudioDeviceInfo> input_devices;
    QList<QAudioDeviceInfo> output_devices;
    CODEC recordCodec;
    QFileDialog *fileBrowser;
    QString recordPath;

    QVariant boxValue(const QComboBox *box);
    void displayInputDeviceProperties(QAudioDeviceInfo);
    void displayOutputDeviceProperties(QAudioDeviceInfo);
    void readSettingsFromXML();
    int getBoxIndex(QComboBox*, QString*);
    int getBoxIndex(QComboBox*, int);
    void setBoxIndex(QComboBox*, int);
    void initSettingsValues();
    void setFormatProperties();

    void showEvent(QShowEvent * event);

private slots:
    void applySettings();
    void cancelSetting();
    void changeInputDevice(int);
    void changeOutputDevice(int);
    void selectRecordPath();

    //overrided

};

#endif // SETTINGS_H
