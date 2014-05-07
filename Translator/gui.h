#ifndef GUI_H
#define GUI_H

#include <QMainWindow>
#include <QDebug>
#include <QTimer>
#include <QListWidgetItem>
#include <QInputDialog>
#include <QMessageBox>
#include "settings.h"

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
    void receiverTimerStart();
    void receiverTimerStop();

    //speaker
    void setDataSent(int);
    int getVolume();
    int getBroadcastingPort();
    void broadcastTimerStart();
    void broadcastTimerStop();

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
    bool receiving;  //client status
    Settings *settings;

signals:
    //signals for listenner
    void startPlayback();
    void stopPlayback();
    void volumeChanged();
    void portChanged(int);

    //signals for broadcast
    void startButtonPressed();
    void stopButtonPressed();

public slots:
    //slots for listenner
    void playbackButtonPushed();
    void updateTime();
    void volumeChangedSlot();
    void getItemData(QListWidgetItem*);
    void addNewChannel();

    //slots for broadcast
    void btn();
    void changeBroadcastingPort();
    void updateBroadcastTime();

    //general
    void menuTriggered(QAction* action);


};

#endif // GUI_H
