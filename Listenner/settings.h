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
#include <QMutex>

#include "noaudiodeviceexception.h"

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

    QAudioDeviceInfo getOutputDevice() const;
    QHostAddress getServerAddress() const;
    qint32 getServerPort() const;
    qint32 getClientPort() const;
    void setClientPort(const int);
    qint32 getClientType() const;
    qint32 getClientId() const;
    void setClientId(const qint32);
    CODEC getRecordCodec() const;
    QString getRecordPath() const;

    bool setServerAddress(QString);

private:
    Ui::Settings *ui;

    QString outputDeviceName;

    QHostAddress *address;
    qint32 serverPort;
    qint32 clientType;
    qint32 clientPort;
    qint32 clientId;

    QList<QAudioDeviceInfo> output_devices;
    QAudioDeviceInfo selectedOutputDevice;
    QAudioDeviceInfo activeOutputDevice;
    CODEC recordCodec;
    QFileDialog *fileBrowser;
    QString recordPath;

    QVariant boxValue(const QComboBox *box);
    void readSettingsFromXML();
    int getBoxIndex(QComboBox*, QString*) const;
    int getBoxIndex(QComboBox*, int) const;
    void setBoxIndex(QComboBox*, int);
    void initSettingsValues();

    bool checkIpAddress(QString);

private slots:
    void applySettings();
    void cancelSetting();
    void changeDevice(int);
    void selectRecordPath();
};

#endif // SETTINGS_H
