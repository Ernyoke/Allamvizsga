#include "gui.h"
#include <QApplication>
#include <QUdpSocket>
#include <QTimer>
#include <QByteArray>
#include "managevoice.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QUdpSocket socket;
    ManageVoice voice(&socket);
    voice.showGUI();

    return a.exec();
}
