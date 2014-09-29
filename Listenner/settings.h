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

    QAudioFormat getListennerAudioFormat();
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

    devinfo xml_outdev;

    Ui::Settings *ui;
    QAudioFormat formatListenner;
    QList<QAudioDeviceInfo> output_devices;
    QAudioDeviceInfo selectedDevice;
    CODEC recordCodec;
    QFileDialog *fileBrowser;
    QString recordPath;

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
    void changeDevice(int);
    void selectRecordPath();
};

#endif // SETTINGS_H
