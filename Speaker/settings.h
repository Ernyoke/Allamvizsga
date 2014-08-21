#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>
#include <QAudioDeviceInfo>
#include <QComboBox>
#include <QDebug>

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

private:
    Ui::Settings *ui;
    QAudioFormat formatSpeaker;
    QList<QAudioDeviceInfo> input_devices;
    QAudioDeviceInfo selectedDevice;

    QVariant boxValue(const QComboBox *box);

    void displayDeviceProperties(QAudioDeviceInfo);

private slots:
    void applySettings();
    void cancelSetting();
    void changeDevice(int index);
};

#endif // SETTINGS_H
