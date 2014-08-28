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
    void setDataReceived(int);
    void setRecordAudioDev(RecordAudio *record);
    void receiverTimerStart();
    void receiverTimerStop();
    int getVolume();
    int getPort();
    void changeRecordButtonState(RecordAudio::STATE);
    void changePauseButtonState(RecordAudio::STATE);
    void changePlayButtonState(bool isPlaying);

    //speaker
    void setDataSent(int);
    int getBroadcastingPort();
    void broadcastTimerStart();
    void broadcastTimerStop();
    void changeBroadcastButtonState(bool);

    //both
    void updateSpeed();
    Settings *getSettings();

private:
    Ui::GUI *ui;
    //listenner
    QTimer timer;
    long dataSize;
    long dataPerSec;
    long cntTime;
    QList<QListWidgetItem*> *channels;

    //speaker
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
    void volumeChanged();
    void startRecord();
    void stopRecord();
    void pauseRecord();
    void changePlayBackState();
    void portChanged(int);

    //signals for broadcast
    void broadcastStateChanged();

public slots:
    //slots for listenner
    void playbackButtonPushed();
    void startRecordPushed();
    void pauseRecordPushed();
    void updateTime();
    void volumeChangedSlot();
    void getItemData(QListWidgetItem*);
    void addNewChannel();

    //slots for broadcast
    void btn();
    void changeBroadcastingPort();
    void updateBroadcastTime();

    //general
    void showSettings();
    void menuTriggered(QAction*);


};

#endif // GUI_H
