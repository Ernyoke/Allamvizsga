#ifndef GUI_H
#define GUI_H

#include <QMainWindow>
#include <QMessageBox>
#include <QInputDialog>
#include <QTimer>
#include "settings.h"
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

    Settings* getSettings();
    int getBroadcastingPort();

    void setDataSent(int);
    void changeBroadcastButtonState(bool);


private:
    Ui::GUI *ui;
    Settings *settings;
    int broadcasting_port;
    long broadcastDataSize;
    long broadcastDataPerSec;
    long cntBroadcastTime;

    QTimer broadcastTimer;

signals:
    void broadcastStateChanged();

public slots:
    void btn();
    void menuTriggered(QAction*);
    void changeBroadcastingPort();
    void broadcastTimerStart();
    void broadcastTimerStop();
    void updateBroadcastTime();

};

#endif // GUI_H
