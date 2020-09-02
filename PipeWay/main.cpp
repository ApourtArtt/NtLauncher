#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QSharedMemory shared("c2a4b6d8-1e3f-1c3a-1b3d-1e3f5a7c9b1d");
    if(!shared.create( 512, QSharedMemory::ReadWrite))
    {
        QLocalSocket *client = new QLocalSocket();
        client->connectToServer("AyugraLauncher", QIODevice::ReadWrite);
        exit(0);
    }
    MainWindow w;
    w.show();
    return a.exec();
}
