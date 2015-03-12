#ifndef GUI_H
#define GUI_H

#include <QMainWindow>
#include <QDebug>
#include <QTimer>
#include <QListWidgetItem>
#include <QInputDialog>
#include <QMessageBox>
#include <QCloseEvent>
#include <QPointer>
#include "settings.h"
#include "recordaudio.h"
#include "listener.h"
#include "logindialog.h"
#include "servercommunicator.h"
#include "channelmodel.h"
#include "channellistexception.h"
#include "addnewchannelfromgui.h"

namespace Ui {
class GUI;
}

class GUI : public QMainWindow
{
    Q_OBJECT

public:
    explicit GUI(QWidget *parent = 0);
    ~GUI();

    //listenner
    Listener *listener;
    void setRecordAudioDev(RecordAudio *record);
    void receiverTimerStart();
    void receiverTimerStop();
    int getVolume();


    //both
    void updateSpeed();
    void login();

private:
    Ui::GUI *ui;
    //listenner
    QTimer timer;
    long dataSize;
    long dataPerSec;
    long cntTime;
    int broadcasting_port;

    //both
    LoginDialog* loginDialog;
    Settings* settings;
    AddNewChannelFromGui *addNewChannelMan;
    QPointer<ServerCommunicator> serverCommunicator;
    QPointer<ChannelModel> channelModel;

    void initialize();


protected:
    void closeEvent(QCloseEvent *event);

signals:
    //signals for listenner
    void volumeChanged();
    void startRecord();
    void stopRecord();
    void pauseRecord();
    void changePlayBackState(QSharedPointer<ChannelInfo>);
    void channelChanged(QSharedPointer<ChannelInfo>);
    void logout();

public slots:
    //slots for listenner
    void playbackButtonPushed();
    void startRecordPushed();
    void pauseRecordPushed();
    void updateTime();
    void volumeChangedSlot();
    void changeChannelOnDoubleClick(QModelIndex);
    void addNewChannel();

    void changeRecordButtonState(RecordAudio::STATE);
    void changePauseButtonState(RecordAudio::STATE);
    void changePlayButtonState(bool isPlaying);
    void setDataReceived(int);

    void setRecordFileName(QString filename);
    void deleteChannel();


    //general
    void menuTriggered(QAction*);
    void showErrorMessage(QString);


};

#endif // GUI_H
