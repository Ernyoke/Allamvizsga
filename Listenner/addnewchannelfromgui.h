#ifndef ADDNEWCHANNELFROMGUI_H
#define ADDNEWCHANNELFROMGUI_H

#include <QDialog>
#include <QAudioDeviceInfo>
#include <QComboBox>
#include <QDebug>

#include "channelinfo.h"

namespace Ui {
class AddNewChannelFromGui;
}

class AddNewChannelFromGui : public QDialog
{
    Q_OBJECT

public:
    explicit AddNewChannelFromGui(QAudioDeviceInfo deviceInfo, QWidget *parent = 0);
    ~AddNewChannelFromGui();

private:
    Ui::AddNewChannelFromGui *ui;

    QAudioDeviceInfo deviceInfo;

    void displayDeviceProperties();
    int getBoxIndex(QComboBox *box, QString *content);
    QVariant boxValue(const QComboBox *box);

    bool startBtnStatus;

private slots:
    void createNewChannel();
    void fieldChannged();

signals:
    void newUserCreatedChannel(ChannelInfo);
};

#endif // ADDNEWCHANNELFROMGUI_H
