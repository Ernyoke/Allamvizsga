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
    QByteArray sendBuffer, receiveBuffer;
    int ID;
    GUI w;

    w.show();

    ManageVoice voice(&socket, &w);


    return a.exec();
}
