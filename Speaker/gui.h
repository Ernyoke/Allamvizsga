#ifndef GUI_H
#define GUI_H

#include <QMainWindow>
#include <QMessageBox>
#include <QInputDialog>
#include <QTimer>
#include <QCloseEvent>
#include <QThread>
#include <QHostAddress>
#include "settings.h"
#include "speaker.h"
#include "testspeaker.h"
#include "logindialog.h"
#include "newchanneldialog.h"
#include "servercommunicator.h"
#include <QDebug>

namespace Ui {
class GUI;
}

class GUI : public QMainWindow
{
    Q_OBJECT

public:
    explicit GUI(QWidget *parent = 0);
    ~GUI();

    AbstractSpeaker *speakerWorker;
    QThread *speakerThread;

    void login();

private:
    Ui::GUI *ui;
    Settings *settings;
    long broadcastDataSize;
    long broadcastDataPerSec;
    long cntBroadcastTime;

    bool isRecording;

    QTimer broadcastTimer;

    LoginDialog *loginDialog;
    NewChannelDialog *newChannelDialog;
    ServerCommunicator *serverCommunicator;

    void stopChannel();
    void init();

protected:
    void closeEvent(QCloseEvent *event);

signals:
    //this signal is emited when the user enters a valid port and starts the broadcast
    void startBroadcast(QAudioFormat speakerFormat, QAudioDeviceInfo device,
                        QHostAddress serverAddress, qint32 broadcasting_port, qint32 clientId);
    void stopBroadcast();
    void stopSpeaker();

    //signals for both
    void sendLogoutRequest();

    //signals emited when server is down
    void stopSpeakingSD();

    //
    void stopSpeakerWorker();

private slots:
    void setDataSent(int);
    void changeBroadcastButtonState(bool);
    void startBroadcast();
    void menuTriggered(QAction*);
    void broadcastTimerStart();
    void broadcastTimerStop();
    void updateBroadcastTime();
    void errorMessage(QString);
    void startNewChannel();

    void serverDownHandle();

};

#endif // GUI_H
