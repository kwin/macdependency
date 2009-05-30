#ifndef DEPENDENCYAPPLICATION_H
#define DEPENDENCYAPPLICATION_H

#include <QtGui/QApplication>
#include "mainwindow.h"


class DependencyApplication : public QApplication
{
    Q_OBJECT
public:
    DependencyApplication(int & argc, char ** argv);
protected:
    bool event(QEvent*);
    MainWindow* getMainWindow();
};

#endif // DEPENDENCYAPPLICATION_H
