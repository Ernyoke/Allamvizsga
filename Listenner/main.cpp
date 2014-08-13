#include "gui.h"
#include <QApplication>
#include "listener.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    GUI w;
    Listener listen(&w);
    w.show();

    return a.exec();
}
