#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>
#include <QSettings>
#include <QDir>
#include <QHostAddress>
#include <QFileDialog>

namespace Ui {
class Settings;
}

class Settings : public QDialog
{
    Q_OBJECT

public:
    explicit Settings(QWidget *parent = 0);
    ~Settings();

    static const QString appName_label;
    static const QString organization_label;
    static const QString bcast_label;
    static const QString log_label;
    static const QString logPath_label;

private:
    Ui::Settings *ui;
    QSettings *settings;
    QString broadcastAddress;
    QString logPath;
    bool log;

    void readSettings();
    bool checkIpAddress(QString&);
    void showErrorMessage(QString);

private slots:
    void saveSettings();
    void browse();

signals:
    void broadcastAddressChanged(QString);
    void packetLogStarted(QString);
    void packetLogStopped();
};

#endif // SETTINGS_H
