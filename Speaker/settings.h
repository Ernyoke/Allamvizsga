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
#include <QPointer>
#include <QReadWriteLock>

#include "noaudiodeviceexception.h"

namespace Ui {
class Settings;
}

class Settings : public QDialog
{
    Q_OBJECT

public:
    explicit Settings(QWidget *parent = 0);
    ~Settings();

    QAudioDeviceInfo getInputDevice() const;
    QHostAddress getServerAddress() const;
    quint16 getServerPort() const;
    quint16 getClientPort() const;
    quint16 getClientPortForSound() const;
    quint32 getClientType() const;
    quint32 getClientId() const;
    void setClientId(const quint32);
    void setClientPort(const qint32);

    bool setServerAddress(const QString);

    bool testMode() const;


private:
    Ui::Settings *ui;

    QString inputDeviceName;

    QHostAddress *address;
    quint16 serverPort;
    quint32 clientType;
    quint16 clientPort;
    quint16 clientPortForSound;
    quint32 clientId;

    QList<QAudioDeviceInfo> input_devices;
    QAudioDeviceInfo selectedInputDevice;
    QAudioDeviceInfo activeInputDevice;
    int inputDeviceCounter;

    QVariant boxValue(const QComboBox *box);

    void readSettingsFromXML();
    int getBoxIndex(QComboBox*, QString*);
    int getBoxIndex(QComboBox*, int);
    void setBoxIndex(QComboBox*, int);
    void initSettingsValues();
    void setFormatProperties();

    bool checkIpAddress(QString &);

private slots:
    void applySettings();
    void cancelSetting();
    void changeDevice(int index);
};

#endif // SETTINGS_H
