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
#include <QThread>
#include <QSettings>
#include <QAudioDeviceInfo>

#include "settings.h"
#include "recordaudio.h"
#include "listener.h"
#include "logindialog.h"
#include "servercommunicator.h"
#include "channelmodel.h"
#include "channellistexception.h"
#include "addnewchannelfromgui.h"
#include "invalididexception.h"

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
    Listener *listenerWorker;
    QThread *listenerThread;

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
    bool isPlaying;
    bool doubleClickEnabled;

    //both
    LoginDialog* loginDialog;
    QSettings* settings;
    AddNewChannelFromGui *addNewChannelMan;
    QPointer<ServerCommunicator> serverCommunicator;
    QPointer<ChannelModel> channelModel;

    void initialize();
    void createListenerThread();
    QAudioDeviceInfo getOutputDevice() const;



protected:
    void closeEvent(QCloseEvent *event);

signals:
    //signals for listenner
    void volumeChanged(qreal);
    void startRecord(Settings::CODEC, QString);
    void stopRecord();
    void pauseRecord();
    void startListening(QSharedPointer<ChannelInfo> channel, QAudioDeviceInfo device, QHostAddress serverAddress, qreal volume);
    void stopListening();

    //signals for both
    void sendLogoutRequest();

    //signals emited when server is down
    void stopPlaybackSD();

public slots:
    //slots for listenner
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
    void changePlayButtonState(bool isPlaying);
    void setDataReceived(int);
    void startNewChannelOnDistroy();

    //general
    void menuTriggered(QAction*);
    void showErrorMessage(QString);
    void serverDownHandle();

};

#endif // GUI_H
