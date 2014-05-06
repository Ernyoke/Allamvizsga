#include "gui.h"
#include <QApplication>
#include "listenner.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    GUI w;
    Listenner *listen = new Listenner(&w);
    w.show();

    return a.exec();
}
