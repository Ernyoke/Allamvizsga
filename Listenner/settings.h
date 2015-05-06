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
#include <QSettings>

#include "noaudiodeviceexception.h"

namespace Ui {
class Settings;
}

class Settings : public QDialog
{
    Q_OBJECT

public:
    enum CODEC {WAV};

    static const QString appname_label;
    static const QString organization_label;
    static const QString record_codec_label;
    static const QString record_path_label;
    static const QString output_device_label;
    static const QString server_address_label;

    static const int CLIENT_TYPE;


    explicit Settings(QWidget *parent = 0);
    ~Settings();

    static bool checkIpAddress(const QString);

private:
    Ui::Settings *ui;

    QSettings *settings;
    QFileDialog *fileBrowser;
    QList<QAudioDeviceInfo> output_devices;
    QList<QString> recordCodecs;
    QString recordPath;

    int getBoxIndex(QComboBox*, QString*) const;
    int getBoxIndex(QComboBox*, int) const;
    void setBoxIndex(QComboBox*, int);
    QVariant boxValue(const QComboBox *box);
    void initSettingsValues();

private slots:
    void applySettings();
    void cancelSettings();
    void selectRecordPath();
};

#endif // SETTINGS_H
