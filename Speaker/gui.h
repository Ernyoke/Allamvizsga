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



private:
    Ui::GUI *ui;
    Settings *settings;
    int broadcasting_port;
    long broadcastDataSize;
    long broadcastDataPerSec;
    long cntBroadcastTime;

    QTimer broadcastTimer;

    LoginDialog *loginDialog;

    void login();

protected:
    void closeEvent(QCloseEvent *event);

signals:
    //this signal is emited when the user enters a valid port and starts the broadcast
    void broadcastStateChanged(QString, QString);
    void logout();

private slots:
    void setDataSent(int);
    void changeBroadcastButtonState(bool);
    void btn();
    void menuTriggered(QAction*);
    void changeBroadcastingPort();
    void broadcastTimerStart();
    void broadcastTimerStop();
    void updateBroadcastTime();
    void errorMessage(QString);

};

#endif // GUI_H
