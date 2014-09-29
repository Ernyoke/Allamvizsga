#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>
#include <QAudioDeviceInfo>
#include <QComboBox>
#include <QDebug>
#include <QFile>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>

namespace Ui {
class Settings;
}

class Settings : public QDialog
{
    Q_OBJECT

public:
    explicit Settings(QWidget *parent = 0);
    ~Settings();

    QAudioFormat getSpeakerAudioFormat();
    QAudioDeviceInfo getInputDevice();

private:
    Ui::Settings *ui;

    struct devinfo {
        QString dev_name;
        QString codec;
        int sample_rate;
        int channels;
        int sample_size;
    };

    devinfo xml_indev;

    QAudioFormat formatSpeaker;
    QList<QAudioDeviceInfo> input_devices;
    QAudioDeviceInfo selectedDevice;

    QVariant boxValue(const QComboBox *box);

    void displayDeviceProperties(QAudioDeviceInfo);
    void readSettingsFromXML();
    int getBoxIndex(QComboBox*, QString*);
    int getBoxIndex(QComboBox*, int);
    void setBoxIndex(QComboBox*, int);
    void initSettingsValues();
    void setFormatProperties();

private slots:
    void applySettings();
    void cancelSetting();
    void changeDevice(int index);
};

#endif // SETTINGS_H
