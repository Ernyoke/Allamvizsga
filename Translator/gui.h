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
#include "testspeaker.h"
#include "logindialog.h"
#include "servercommunicator.h"
#include "newchanneldialog.h"
#include "channelmodel.h"
#include "addnewchannelfromgui.h"
#include "noaudiodeviceexception.h"

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
    void broadcastTimerStart();
    void broadcastTimerStop();

    //both
    void login();
    void updateSpeed();

protected:
    void closeEvent(QCloseEvent *event);

private:
    Ui::GUI *ui;

    QThread *listenerThread;
    QThread *speakerThread;

    bool isSpeakerRunning;
    bool isListenerRunning;
    bool doubleClickEnabled;

    bool isLogging;
    QFile *packetLoggerFile;
    QMutex* logMutex;

    //listenner
    Listener *listenerWorker;
    QTimer timer;
    long dataSize;
    long dataPerSec;
    long cntTime;

    QSharedPointer<ChannelInfo> selectedChannel;

    //speaker
    AbstractSpeaker *speakerWorker;
    QTimer broadcastTimer;
    long broadcastDataSize;
    long broadcastDataPerSec;
    long cntBroadcastTime;

    //both
    QSettings *settings;
    LoginDialog *loginDialog;
    NewChannelDialog *newChannelDialog;
    QPointer<ServerCommunicator> serverCommunicator;
    QPointer<ChannelModel> channelModel;


    void initialize();
    void stopChannel();
    void createListenerThread();
    QAudioDeviceInfo getInputDevice() const;
    QAudioDeviceInfo getOutputDevice() const;

signals:
    //signals for listenner
    void volumeChanged(qreal);
    void startRecord(QString, QString);
    void stopRecord();
    void pauseRecord();
    void startListening(QSharedPointer<ChannelInfo> channel, QAudioDeviceInfo device, QHostAddress serverAddress, qreal volume);
    void stopListening();

    //signals for broadcast
    //this signal is emited when the user enters a valid port and starts the broadcast
    void startBroadcast(QAudioFormat speakerFormat, QAudioDeviceInfo device,
                        QHostAddress serverAddress, qint32 broadcasting_port, qint32 clientId);
    void stopBroadcast();

    //signals for both
    void sendLogoutRequest();
    void startPacketLog(QMutex*, QFile*);
    void stopPacketLog();

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
    void addNewLocalChannel();

    void changeRecordButtonState(RecordAudio::STATE);
    void changePauseButtonState(RecordAudio::STATE);
    void changePlayButtonState(bool isPlaying);
    void setDataReceived(int);
    void startNewChannelOnDistroy();
    void channelStopedByServer(qint32);


    //slots for broadcast
    void startNewChannel();
    void startBroadcast();
    void updateBroadcastTime();
    void changeBroadcastButtonState(bool);
    void setDataSent(int);

    //general
    void menuTriggered(QAction*);
    void errorMessage(QString);
    void serverDownHandle();
    void preparePacketLog(QString);
    void finalizePacketLog();


};

//Q_DECLARE_METATYPE(QSharedPointer<ChannelInfo>)

#endif // GUI_H
