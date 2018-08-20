#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    QTimer::singleShot(10*60*1000, &a, a.quit);
    return a.exec();
}
