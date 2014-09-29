#ifndef GUI_H
#define GUI_H

#include <QMainWindow>
#include <QDebug>
#include <QTimer>
#include <QListWidgetItem>
#include <QInputDialog>
#include <QMessageBox>
#include "settings.h"
#include "recordaudio.h"
#include "listener.h"
#include "speaker.h"

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
    int getPort();
    void changeRecordButtonState(RecordAudio::STATE);
    void changePauseButtonState(RecordAudio::STATE);

    //speaker
    int getBroadcastingPort();
    void broadcastTimerStart();
    void broadcastTimerStop();

    //both
    void updateSpeed();
    Settings *getSettings();

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
    QList<QListWidgetItem*> *channels;

    //speaker
    Speaker *speaker;
    QTimer broadcastTimer;
    long broadcastDataSize;
    long broadcastDataPerSec;
    long cntBroadcastTime;
    int broadcasting_port;

    //both
    Settings *settings;
    QStatusBar *sBar;

    void initialize();

signals:
    //signals for listenner
    void volumeChanged(int);
    void startRecord();
    void stopRecord();
    void pauseRecord();
    void changePlayBackState(int);
    void portChanged(int);
    void stopListener();

    //signals for broadcast
    void broadcastStateChanged(int);
    void stopSpeaker();

public slots:
    //slots for listener
    void playbackButtonPushed();
    void startRecordPushed();
    void pauseRecordPushed();
    void updateTime();
    void volumeChangedSlot();
    void getItemData(QListWidgetItem*);
    void addNewChannel();

    void changePlayButtonState(bool isPlaying);
    void setDataReceived(int);


    //slots for broadcast
    void btn();
    void changeBroadcastingPort();
    void updateBroadcastTime();
    void changeBroadcastButtonState(bool);
    void setDataSent(int);

    //general
    void showSettings();
    void menuTriggered(QAction*);


};

#endif // GUI_H
