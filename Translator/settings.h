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

    QAudioDeviceInfo getInputDevice() const;
    QAudioDeviceInfo getOutputDevice() const;
    CODEC getRecordCodec() const;
    QString getRecordPath() const;
    QHostAddress getServerAddress() const;
    qint32 getServerPort() const;
    void setClientPort(const qint32);
    qint32 getClientPort() const;
    qint32 getClientPortForSound() const;
    qint32 getClientType() const;
    qint32 getClientId() const;
    void setClientId(const quint32);

    bool setServerAddress(const QString);

    bool testMode() const;

private:

    Ui::Settings *ui;

    QString inputDeviceName;
    QString outputDeviceName;

    QHostAddress *address;
    qint32 serverPort;
    qint32 clientType;
    qint32 clientPort;
    qint32 clientPortForSound;
    qint32 clientId;

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
    int getBoxIndex(QComboBox*, QString*) const;
    int getBoxIndex(QComboBox*, int) const;
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
