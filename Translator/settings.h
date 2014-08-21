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

    QAudioFormat getSpeakerAudioFormat();
    QAudioFormat getListennerAudioFormat();
    QAudioDeviceInfo getInputDevice();
    QAudioDeviceInfo getOutputDevice();

private:
    Ui::Settings *ui;
    QAudioFormat formatSpeaker;
    QAudioFormat formatListenner;
    QAudioDeviceInfo selectedInputDevice;
    QAudioDeviceInfo selectedOutputDevice;
    QList<QAudioDeviceInfo> input_devices;
    QList<QAudioDeviceInfo> output_devices;

    QVariant boxValue(const QComboBox *box);

    void displayInputDeviceProperties(QAudioDeviceInfo);
    void displayOutputDeviceProperties(QAudioDeviceInfo);

private slots:
    void applySettings();
    void cancelSetting();
};

#endif // SETTINGS_H
