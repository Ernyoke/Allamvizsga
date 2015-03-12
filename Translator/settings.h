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
#include <QHostAddress>

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

    QAudioDeviceInfo getInputDevice();
    QAudioDeviceInfo getOutputDevice();
    CODEC getRecordCodec();
    QString getRecordPath();
    QHostAddress *getServerAddress();
    quint16 getServerPort();
    quint16 getClientPort();
    quint16 getClientPortForSound();
    quint32 getClientType();
    quint32 getClientId();
    void setClientId(quint32);

    bool setServerAddress(QString);

private:

    Ui::Settings *ui;

    QString inputDeviceName;
    QString outputDeviceName;

    QHostAddress *address;
    quint16 serverPort;
    quint32 clientType;
    quint16 clientPort;
    quint16 clientPortForSound;
    quint32 clientId;

    QAudioDeviceInfo selectedInputDevice;
    QAudioDeviceInfo activeInputDevice;
    QAudioDeviceInfo selectedOutputDevice;
    QAudioDeviceInfo activeOutputDevice;

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

    void showEvent(QShowEvent * event);

    bool checkIpAddress(QString);

private slots:
    void applySettings();
    void cancelSetting();
    void changeInputDevice(int);
    void changeOutputDevice(int);
    void selectRecordPath();

    //overrided

};

#endif // SETTINGS_H
