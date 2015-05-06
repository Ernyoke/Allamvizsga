#ifndef GUI_H
#define GUI_H

#include <QMainWindow>
#include <QMessageBox>
#include <QKeyEvent>
#include <QCloseEvent>
#include <QThread>
#include <QNetworkInterface>
#include <QDateTime>
#include <QSettings>
#include <QPointer>
#include <QFile>
#include "acceptdata.h"
#include "manageclients.h"
#include "channelmodel.h"
#include "clientmodel.h"
#include "showclients.h"
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

private:
    Ui::GUI *ui;
    QPointer<ManageClients> manageClients;
    QSettings *settings;

    ChannelModel *channelModel;
    ClientModel *clientModel;
    ShowClients *showClients;

    AcceptData *soundWorker;
    QThread *soundThread;

    QMutex *mutex;
    QFile *packetLoggerFile;
    bool isLogging;

    void init();
    bool checkUsedPorts(int, int);
    QString generateTimeStamp();

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void showClientList();
    void showSettings();
    void logClientConnected(ClientInfo*);
    void logClientDisconnected(qint32 id);
    void logClientTimedOut(qint32 id);
    void preparePacketLog(QString);
    void finalizePacketLog();

signals:
    void stopSoundWorker();
    void startPacketLog(QFile*);
    void stopPacketLog();
    void errorMessage(QString);

};

#endif // GUI_H
