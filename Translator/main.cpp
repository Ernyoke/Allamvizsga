#include "gui.h"
#include "listener.h"
#include "speaker.h"
#include <QApplication>
#include <QUdpSocket>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    GUI w;
    QUdpSocket socket;
    Listener *listen = new Listener(&w);
    Speaker *speak = new Speaker(&socket, &w);
    listen->start();
    speak->start();
    w.show();

    return a.exec();
}
