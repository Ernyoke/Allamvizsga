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
#include "speaker.h"
#include "logindialog.h"
#include "servercommunicator.h"
#include "newchanneldialog.h"
#include "channelmodel.h"
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
    void setRecordAudioDev(RecordAudio *record);
    void receiverTimerStart();
    void receiverTimerStop();
    int getVolume();

    //speaker
    int getBroadcastingPort();
    void broadcastTimerStart();
    void broadcastTimerStop();

    //both
    void login();
    void updateSpeed();
    Settings *getSettings();

protected:
    void closeEvent(QCloseEvent *event);

private:
    Ui::GUI *ui;

    QThread *threadListener;
    QThread *threadSpeaker;

    //listenner
    Listener *listener;
    QTimer timer;
    long dataSize;
    long dataPerSec;
    long cntTime;

    //speaker
    Speaker *speaker;
    QTimer broadcastTimer;
    long broadcastDataSize;
    long broadcastDataPerSec;
    long cntBroadcastTime;
    int broadcasting_port;

    //both
    Settings *settings;
    LoginDialog *loginDialog;
    NewChannelDialog *newChannelDialog;
    AddNewChannelFromGui *addNewChannelMan;
    QPointer<ServerCommunicator> serverCommunicator;
    QPointer<ChannelModel> channelModel;


    void initialize();
    void stopChannel();

signals:
    //signals for listenner
    void volumeChanged(int);
    void startRecord();
    void stopRecord();
    void pauseRecord();
    void changePlayBackState(QSharedPointer<ChannelInfo>);
    void portChanged(int);
    void stopListener();
    void finalRecordName(bool, QString);

    //signals for broadcast
    void broadcastStateChanged(QAudioFormat);
    void stopSpeaker();

    //signals for both
    void sendLogoutRequest();

    //signals emited when server is down
    void stopPlaybackSD();
    void stopSpeakingSD();

public slots:
    //slots for listener
    void playbackButtonPushed();
    void startRecordPushed();
    void pauseRecordPushed();
    void deleteChannel();
    void updateTime();
    void volumeChangedSlot();
    void changeChannelOnDoubleClick(QModelIndex);
    void addNewChannel();
    void changeRecordButtonState(RecordAudio::STATE);
    void changePauseButtonState(RecordAudio::STATE);
    void changePlayButtonState(bool);
    void setDataReceived(int);

    void setRecordFileName(QString filename);


    //slots for broadcast
    void startNewChannel();
    void startBroadcast();
    void changeBroadcastingPort();
    void updateBroadcastTime();
    void changeBroadcastButtonState(bool);
    void setDataSent(int);

    //general
    void showSettings();
    void menuTriggered(QAction*);
    void showErrorMessage(QString);
    void serverDownHandle();


};

//Q_DECLARE_METATYPE(QSharedPointer<ChannelInfo>)

#endif // GUI_H
