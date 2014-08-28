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

public slots:
    //slots for listenner
    void playbackButtonPushed();
    void startRecordPushed();
    void pauseRecordPushed();
    void updateTime();
    void volumeChangedSlot();
    void getItemData(QListWidgetItem*);
    void addNewChannel();

    //general
    void menuTriggered(QAction*);


};

#endif // GUI_H
