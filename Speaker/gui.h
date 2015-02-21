#ifndef GUI_H
#define GUI_H

#include <QMainWindow>
#include <QMessageBox>
#include <QInputDialog>
#include <QTimer>
#include <QCloseEvent>
#include "settings.h"
#include "managevoice.h"
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

    ManageVoice *speaker;

    Settings* getSettings();
    int getBroadcastingPort();

    bool isChannelRegistered;
    bool isRecording;

    void login();


private:
    Ui::GUI *ui;
    Settings *settings;
    int broadcasting_port;
    long broadcastDataSize;
    long broadcastDataPerSec;
    long cntBroadcastTime;

    QTimer broadcastTimer;

    LoginDialog *loginDialog;
    NewChannelDialog *newChannelDialog;
    ServerCommunicator *serverCommunicator;

protected:
    void closeEvent(QCloseEvent *event);

signals:
    //this signal is emited when the user enters a valid port and starts the broadcast
    void broadcastStateChanged(QAudioFormat);
    void logout();

private slots:
    void setDataSent(int);
    void changeBroadcastButtonState(bool);
    void startBroadcast();
    void menuTriggered(QAction*);
    void changeBroadcastingPort();
    void broadcastTimerStart();
    void broadcastTimerStop();
    void updateBroadcastTime();
    void errorMessage(QString);
    void startNewChannel();

};

#endif // GUI_H
