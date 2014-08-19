#include "gui.h"
#include <QApplication>
#include "listener.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Listener listen;
    listen.showGUI();

    return a.exec();
}
