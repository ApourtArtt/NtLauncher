#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QProcess *p = new QProcess();
    p->start("cmd.exe");
    p->waitForStarted(5000);
    MainWindow w;
    w.show();
    return a.exec();
}
