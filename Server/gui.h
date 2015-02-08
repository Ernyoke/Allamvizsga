#ifndef GUI_H
#define GUI_H

#include <QMainWindow>
#include <QMessageBox>
#include <QKeyEvent>
#include "acceptdata.h"
#include "manageclients.h"

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
    int channelCounter;
    QMap<int, AcceptData*>channels;
    ManageClients *manageClients;

    bool checkUsedPorts(int, int);

protected:
    void keyPressEvent(QKeyEvent*);

private slots:
    void startChannel();
    void stopChannel();
    void showClients();

};

#endif // GUI_H
