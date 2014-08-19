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

private:
    Ui::Settings *ui;
    QAudioFormat formatListenner;

    QVariant boxValue(const QComboBox *box);

private slots:
    void applySettings();
    void cancelSetting();
};

#endif // SETTINGS_H
