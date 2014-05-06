#include "gui.h"
#include "listenner.h"
#include "speaker.h"
#include <QApplication>
#include <QUdpSocket>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    GUI w;
    QUdpSocket socket;
    Listenner *listen = new Listenner(&w);
    Speaker *speak = new Speaker(&socket, &w);
    listen->start();
    speak->start();
    w.show();

    return a.exec();
}
