#include <QtGui/QApplication>
#include "mainwindow.h"
#include "dependencyapplication.h"

int main(int argc, char *argv[])
{
    DependencyApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
