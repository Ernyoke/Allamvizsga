#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>
#include <QAudioDeviceInfo>
#include <QComboBox>

namespace Ui {
class Settings;
}

class Settings : public QDialog
{
    Q_OBJECT

public:
    explicit Settings(QWidget *parent = 0);
    ~Settings();

    QAudioFormat getListennerAudioFormat();
    QAudioDeviceInfo getOutputDevice();

private:
    Ui::Settings *ui;
    QAudioFormat formatListenner;
    QList<QAudioDeviceInfo> output_devices;
    QAudioDeviceInfo selectedDevice;

    QVariant boxValue(const QComboBox *box);

    void displayDeviceProperties(QAudioDeviceInfo);

private slots:
    void applySettings();
    void cancelSetting();
    void changeDevice(int);
};

#endif // SETTINGS_H
