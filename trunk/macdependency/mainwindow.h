#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QtGui/QDragEnterEvent>
#include <QtGui/QLabel>
#include <QtGui/QComboBox>
#include <QtGui/QItemSelection>
#include <QtGui/QSortFilterProxyModel>

#include "macho.h"
#include "librarytablemodel.h"
#include "symboltablemodel.h"

namespace Ui
{
    class MainWindowClass;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void loadFile(const QString&);

private:
    Ui::MainWindowClass *ui;
    MachO* machO;
    MachOArchitecture* architecture;
    QSortFilterProxyModel libraryFilterModel;
    LibraryTableModel* libraryModel;
    QSortFilterProxyModel symbolFilterModel;
    SymbolTableModel* symbolModel;
    const static QString cpuTypes[MachOHeader::NumCpuTypes];
    const static QString fileTypes[MachOHeader::NumFileTypes];
    enum SymbolFilter {
        SymbolFilterExported = 0,
        SymbolFilterImported,
        SymbolFilterLocal
    };


private:
    void dragEnterEvent(QDragEnterEvent*);
    void dropEvent(QDropEvent*);

    void loadSymbolTable(MachOArchitecture* architecture, QModelIndex& selected);
    void loadLibraries(MachOArchitecture* architecture);
    void resetWidgets();
    int setFileInformation(MachO* file, MachOArchitecture* selectedArchitecture = 0);
    void setArchitectureInformation(MachO* file, MachOArchitecture* architecture);
    void refreshFilter();




private slots:
    void on_actionAbout_triggered();
    void on_actionOpen_triggered();
    void on_architecturesBox_activated(int);
    void on_demangleNamesButton_toggled(bool);
    void on_importButton_toggled(bool);
    void on_exportButton_toggled(bool);

    void changeCurrentLibrary(const QItemSelection & selected, const QItemSelection & deselected);
};


#endif // MAINWINDOW_H
