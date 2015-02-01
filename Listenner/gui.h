#ifndef GUI_H
#define GUI_H

#include <QMainWindow>
#include <QDebug>
#include <QTimer>
#include <QListWidgetItem>
#include <QInputDialog>
#include <QMessageBox>
#include <QCloseEvent>
#include "settings.h"
#include "recordaudio.h"
#include "listener.h"
#include "logindialog.h"

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
    int getPort();


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
    int broadcasting_port;

    //both
    Settings *settings;
    QStatusBar *sBar;

    void initialize();

    void login();

protected:
    void closeEvent(QCloseEvent *event);

signals:
    //signals for listenner
    void volumeChanged();
    void startRecord();
    void stopRecord();
    void pauseRecord();
    void changePlayBackState(int);
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
