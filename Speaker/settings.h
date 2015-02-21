#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>
#include <QAudioDeviceInfo>
#include <QComboBox>
#include <QDebug>
#include <QFile>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QHostAddress>

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
    QHostAddress *getServerAddress();
    quint16 getServerPort();
    quint16 getClientPort();
    quint16 getClientPortForSound();
    quint32 getClientType();
    quint32 getClientId();
    void setClientId(quint32);

    bool setServerAddress(QString);
    QAudioDeviceInfo getDeviceInfo();


private:
    Ui::Settings *ui;

    struct devinfo {
        QString dev_name;
        QString codec;
        quint32 sample_rate;
        quint32 channels;
        quint32 sample_size;
    };

    devinfo xml_indev;

    QHostAddress *address;
    quint16 serverPort;
    quint32 clientType;
    quint16 clientPort;
    quint16 clientPortForSound;
    quint32 clientId;

    QAudioFormat formatSpeaker;
    QList<QAudioDeviceInfo> input_devices;
    QAudioDeviceInfo selectedDevice;

    QVariant boxValue(const QComboBox *box);

    void readSettingsFromXML();
    int getBoxIndex(QComboBox*, QString*);
    int getBoxIndex(QComboBox*, int);
    void setBoxIndex(QComboBox*, int);
    void initSettingsValues();
    void setFormatProperties();

    bool checkIpAddress(QString);

private slots:
    void applySettings();
    void cancelSetting();
    void changeDevice(int index);
};

#endif // SETTINGS_H
