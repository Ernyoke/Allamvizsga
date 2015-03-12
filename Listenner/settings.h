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

    QAudioDeviceInfo getOutputDevice();
    QHostAddress *getServerAddress();
    quint16 getServerPort();
    quint16 getClientPort();
    quint32 getClientType();
    quint32 getClientId();
    void setClientId(quint32);
    CODEC getRecordCodec();
    QString getRecordPath();

    bool setServerAddress(QString);

private:
    Ui::Settings *ui;

    QString outputDeviceName;

    QHostAddress *address;
    quint16 serverPort;
    quint32 clientType;
    quint16 clientPort;
    quint32 clientId;

    QList<QAudioDeviceInfo> output_devices;
    QAudioDeviceInfo selectedOutputDevice;
    QAudioDeviceInfo activeOutputDevice;
    CODEC recordCodec;
    QFileDialog *fileBrowser;
    QString recordPath;

    QVariant boxValue(const QComboBox *box);
    void readSettingsFromXML();
    int getBoxIndex(QComboBox*, QString*);
    int getBoxIndex(QComboBox*, int);
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
