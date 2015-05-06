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
#include <QSettings>

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
    static const QString input_device_label;
    static const QString server_address_label;
    static const QString testmode_label;
    static const QString log_label;
    static const QString logPath_label;

    static const int CLIENT_TYPE;

    explicit Settings(QWidget *parent = 0);
    ~Settings();

    static bool checkIpAddress(const QString);

private:

    Ui::Settings *ui;

    QSettings *settings;

    QList<QAudioDeviceInfo> input_devices;
    QList<QAudioDeviceInfo> output_devices;
    QList<QString> recordCodecs;

    CODEC recordCodec;
    QFileDialog *fileBrowser;
    QString recordPath;

    bool log;
    QString logPath;

    QVariant boxValue(const QComboBox *box);
    int getBoxIndex(QComboBox*, QString*) const;
    int getBoxIndex(QComboBox*, int) const;
    void setBoxIndex(QComboBox*, int);
    void initSettingsValues();


private slots:
    void applySettings();
    void cancelSettings();
    void selectRecordPath();
    void browseLogPath();
    void enableLogPath(bool);

signals:
    void packetLogStarted(QString);
    void packetLogStopped();
};

#endif // SETTINGS_H
