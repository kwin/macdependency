#include "dependencyapplication.h"

DependencyApplication::DependencyApplication(int & argc, char** argv) :
        QApplication (argc, argv)
{
}

bool DependencyApplication::event(QEvent *event)
{
    switch (event->type()) {
        // used in MacOSX to automatically open files (via drop on dock)
    case QEvent::FileOpen:
        getMainWindow()->loadFile(static_cast<QFileOpenEvent *>(
                event)->file());
        return true;
    default:
        return QApplication::event(event);
    }
}

MainWindow* DependencyApplication::getMainWindow() {
    QWidgetList list = topLevelWidgets();
    return static_cast<MainWindow*>(list.at(0));
}
