#ifndef GUI_H
#define GUI_H

#include <QMainWindow>
#include <QDebug>
#include <QTimer>
#include <QListWidgetItem>

namespace Ui {
class GUI;
}

class GUI : public QMainWindow
{
    Q_OBJECT

public:
    explicit GUI(QWidget *parent = 0);
    ~GUI();
    void setDataReceived(int);
    void timerStart();
    void timerStop();
    void updateSpeed();
    int getVolume();


private:
    Ui::GUI *ui;
    QTimer timer;
    long dataSize;
    bool receiving;
    long cntTime;

    QList<QListWidgetItem*> *channels;

signals:
    void startPlayback();
    void stopPlayback();
    void volumeChanged();
    void portChanged(int);

public slots:
    void playbackButtonPushed();
    void updateTime();
    void volumeChangedSlot();
    void getItemData(QListWidgetItem*);


};

#endif // GUI_H
