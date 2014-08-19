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

private:
    Ui::Settings *ui;
    QAudioFormat formatSpeaker;

    QVariant boxValue(const QComboBox *box);

private slots:
    void applySettings();
    void cancelSetting();
};

#endif // SETTINGS_H
