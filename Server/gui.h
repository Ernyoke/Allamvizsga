#ifndef GUI_H
#define GUI_H

#include <QMainWindow>
#include <QMessageBox>
#include <QKeyEvent>
#include <QCloseEvent>
#include <QThread>
#include <QNetworkInterface>
#include <QDateTime>
#include "acceptdata.h"
#include "manageclients.h"
#include "channelmodel.h"
#include "clientmodel.h"
#include "showclients.h"

namespace Ui {
class GUI;
}

class GUI : public QMainWindow
{
    Q_OBJECT

public:
    explicit GUI(QWidget *parent = 0);
    ~GUI();

private:
    Ui::GUI *ui;
    ManageClients *manageClients;

    ChannelModel *channelModel;
    ClientModel *clientModel;
    ShowClients *showClients;

    AcceptData *soundWorker;
    QThread *soundThread;

    bool checkUsedPorts(int, int);
    QString generateTimeStamp();

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void showClientList();
    void logClientConnected(ClientInfo*);
    void logClientDisconnected(qint32 id);
//    void manageTreadFinished();
//    void acceptThreadFinished();

signals:
    void stopSoundWorker();

};

#endif // GUI_H
