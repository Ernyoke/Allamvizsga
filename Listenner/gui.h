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
    int getVolume();

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
    bool receiving;  //client status
    Settings *settings;

signals:
    //signals for listenner
    void startPlayback();
    void stopPlayback();
    void volumeChanged();
    void portChanged(int);

public slots:
    //slots for listenner
    void playbackButtonPushed();
    void updateTime();
    void volumeChangedSlot();
    void getItemData(QListWidgetItem*);
    void addNewChannel();

    //general
    void menuTriggered(QAction*);


};

#endif // GUI_H
