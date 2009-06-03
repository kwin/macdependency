#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "machoexception.h"
#include "modeltest.h"
#include "aboutdialog.h"

#include <QtCore/QList>
#include <QtCore/QUrl>
#include <QtCore/QMetaType>
#include <QtCore/QDateTime>
#include <QtGui/QMessageBox>
#include <QtGui/QFileDialog>


// static variables
const QString MainWindow::cpuTypes[MachOHeader::NumCpuTypes] = { tr("Power PC"), tr("Intel I386"), tr("Power PC 64"), tr("Intel X86-64"), tr("Other") };
const QString MainWindow::fileTypes[MachOHeader::NumFileTypes] = { tr("Object File"), tr("Executable File"), tr("VM Shared Library File"), tr("Core File"),
                                                                   tr("Preloaded Executabble File"), tr("Shared Library"), tr("Dynamic Linker"), tr("Static Library"), tr("Dsym File")};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindowClass), machO(0), architecture(0), libraryModel(0), symbolModel(0)
{
    ui->setupUi(this);
    setAcceptDrops(true);

    libraryFilterModel.setSortRole(LibraryTableModel::SortRole);
    ui->libraryView->setModel(&libraryFilterModel);
    connect(ui->libraryView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)), this, SLOT( changeCurrentLibrary(const QItemSelection &, const QItemSelection &) ) );
    ui->libraryView->sortByColumn(0, Qt::AscendingOrder);

    ui->symbolView->setModel(&symbolFilterModel);
    ui->symbolView->sortByColumn(0, Qt::AscendingOrder);
    symbolFilterModel.setFilterKeyColumn(0);

    refreshFilter();

    resetWidgets();
}

MainWindow::~MainWindow()
{
    if (symbolModel) {
        delete symbolModel;
    }
    if (libraryModel)
        delete libraryModel;
    if (machO)
        delete machO;

    delete ui;
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    // accept just text/uri-list mime format
    if (event->mimeData()->hasFormat("text/uri-list")) {
        event->acceptProposedAction();
    }

}

void MainWindow::dropEvent(QDropEvent* event) {
    QList<QUrl> urlList;
    QString fileName;
    QFileInfo fileInformation;

    if (event->mimeData()->hasUrls()) {
        urlList = event->mimeData()->urls(); // returns list of QUrls

        // if just text was dropped, urlList is empty (size == 0)
        if (urlList.size() > 0) // if at least one QUrl is present in list
        {
            fileName = urlList[0].toLocalFile(); // convert first QUrl to local path
            fileInformation.setFile(fileName); // information about file
            if (fileInformation.isFile() || fileInformation.isDir())
                loadFile(fileName); // if is file, setText
        }
    }
    event->acceptProposedAction();
}

void MainWindow::loadFile(const QString& fileName) {
    resetWidgets();
    try {
        // reset old data
        if (machO) {
            delete machO;
            machO = 0;
            architecture = 0;
        }
        machO = new MachO(fileName);
        loadLibraries(*(machO->getArchitecturesBegin()));

    } catch (MachOException& exc) {
        ui->logBrowser->append(QString(tr("Error loading %1: %2")).arg(fileName).arg(exc.getCause()));
    }
}

void MainWindow::loadLibraries(MachOArchitecture* architecture) {
    this->architecture = architecture;

    if (libraryModel) {
        delete libraryModel;
    }
    libraryModel = new LibraryTableModel(architecture, machO, ui->logBrowser, ui->loadedLibrariesBrowser);
    libraryFilterModel.setSourceModel(libraryModel);
#ifdef QT_DEBUG
    new ModelTest(&libraryFilterModel, this);
#endif
    ui->libraryView->resizeColumnToContents(0);

    // expand view
    QModelIndex rootModelIndex = libraryFilterModel.index(0,0, QModelIndex());
    ui->libraryView->expand(rootModelIndex);

    // select first item
    ui->libraryView->selectionModel()->select(rootModelIndex, QItemSelectionModel::SelectCurrent);
}

void MainWindow::loadSymbolTable(MachOArchitecture* architecture, QModelIndex& selected) {
    LibraryTableModel::LibraryItem* item = libraryModel->getModelIndexData(libraryFilterModel.mapToSource(selected));
    setArchitectureInformation(item->file, architecture);

    if (symbolModel) {
        delete symbolModel;
        symbolModel = 0;
    }
    // check if names should be demangled
    symbolModel = new SymbolTableModel(architecture, ui->demangleNamesButton->isChecked());
    symbolFilterModel.setSourceModel(symbolModel);
#ifdef QT_DEBUG
    new ModelTest(&symbolFilterModel, this);
#endif
    ui->symbolView->resizeRowsToContents();
}

void MainWindow::resetWidgets() {
    ui->logBrowser->clear();
    ui->loadedLibrariesBrowser->clear();

    resetFileInformation();
    resetArchitectureInformation();

    symbolFilterModel.setSourceModel(0);
    libraryFilterModel.setSourceModel(0);
}

void MainWindow::resetFileInformation() {
    ui->fileNameLabel->clear();
    ui->fileTypeLabel->clear();
    ui->sizeLabel->clear();
    ui->lastModifiedLabel->clear();
    ui->architecturesBox->clear();
}

void MainWindow::resetArchitectureInformation() {
    ui->versionLabel->clear();
    ui->nameLabel->clear();
}

void MainWindow::setArchitectureInformation(MachO* file, MachOArchitecture* architecture) {
    if (file == 0 || architecture == 0) {
        resetArchitectureInformation();
        return;
    }

    QString size = tr("%1 KB").arg(file->getSize()/1024);
    if (file->getSize() != architecture->getSize()) {
        size.append(tr(" (Architecture: %1 KB)").arg(architecture->getSize()/1024));
    }
    ui->sizeLabel->setText(size);

    QString name;
    // get version info (first from bundle information)
    QString version = file->getBundleVersion();
    if (!version.isNull()) {
        ui->versionLabel->setText(version);
    } else {
        ui->versionLabel->setText("?");
        // check architecture for version information
        DylibCommand* dynamicLibraryIdCommand = architecture->getDynamicLibIdCommand();
        if (dynamicLibraryIdCommand != 0) {
            time_t timestamp = dynamicLibraryIdCommand->getTimeStamp();
            version = QString(tr("%1 (compatible %2)")).
                                      arg(dynamicLibraryIdCommand->getVersionString(dynamicLibraryIdCommand->getCurrentVersion()))
                                      .arg(dynamicLibraryIdCommand->getVersionString(dynamicLibraryIdCommand->getCompatibleVersion()));
            if (timestamp > 1) {
                version.append(QString(tr(", %1")).arg(QDateTime::fromTime_t(timestamp).toString()));
            }
            ui->versionLabel->setText(version);
            name = dynamicLibraryIdCommand->getName();
        }
    }
    ui->fileTypeLabel->setText(fileTypes[architecture->getHeader()->getFileType()]);

    if (name.isEmpty()) {
        ui->nameLabelLabel->setText("Bundle Name");
        // get bundle name if install name is not available
        name = file->getBundleName();
    } else
        ui->nameLabelLabel->setText("Install Name");
    ui->nameLabel->setText(name);
}

int MainWindow::setFileInformation(MachO* file, MachOArchitecture* selectedArchitecture) {
    if (file == 0) {
        resetFileInformation();
        resetArchitectureInformation();
        return -1;
    }
    ui->fileNameLabel->setText(file->getFileName());
    ui->lastModifiedLabel->setText(QDateTime::fromTime_t(file->getLastModificationDate()).toString(Qt::SystemLocaleShortDate));

    ui->architecturesBox->clear();
    int currentIndex = 0;
    for (std::vector<MachOArchitecture*>::iterator it = file->getArchitecturesBegin(); it != file->getArchitecturesEnd(); ++it)
    {
        ui->architecturesBox->addItem(cpuTypes[(*it)->getHeader()->getCpuType()], QVariant::fromValue((void*)*it));
        if ((*it) == selectedArchitecture || (selectedArchitecture == NULL && currentIndex == 0)) {
            ui->architecturesBox->setCurrentIndex(currentIndex);
        }
        currentIndex++;
    }
    return ui->architecturesBox->currentIndex();
}

void MainWindow::on_architecturesBox_activated(int index)
{
    QModelIndex current =  ui->libraryView->selectionModel()->selectedIndexes().first();
    if (!current.isValid())
        return;

    MachOArchitecture* architecture = (MachOArchitecture*)ui->architecturesBox->itemData(index).value<void*>();

    // if this is the architectures box of the root element -> reload everything with another architecture
    if (current.column() == 0 && current.row() == 0 && !current.parent().isValid() ) {
        ui->loadedLibrariesBrowser->clear();
        ui->logBrowser->clear();
        loadLibraries(architecture);
    }
    // otherwise just reload the symbol table
    else {
        loadSymbolTable(architecture, current);
    }
}

void MainWindow::changeCurrentLibrary(const QItemSelection & selected, const QItemSelection & /*deselected*/)
{
    QModelIndex modelIndex = selected.first().topLeft();
    LibraryTableModel::LibraryItem* item = libraryModel->getModelIndexData(libraryFilterModel.mapToSource(modelIndex));
    setFileInformation(item->file, item->architecture);
    loadSymbolTable(item->architecture, modelIndex);

}

void MainWindow::on_actionAbout_triggered()
{
    AboutDialog dialog;
    dialog.exec();
}

void MainWindow::on_actionOpen_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this);
    if (!fileName.isEmpty())
        loadFile(fileName);
}

void MainWindow::on_demangleNamesButton_toggled(bool /*isChecked*/)
{
    // reload symbols
    QModelIndex current =  ui->libraryView->selectionModel()->selectedIndexes().first();
    if (!current.isValid())
        return;
    loadSymbolTable(this->architecture, current);
}

void MainWindow::on_exportButton_toggled(bool /*isChecked*/)
{
    refreshFilter();
}

void MainWindow::on_importButton_toggled(bool /*isChecked*/)
{
    refreshFilter();
}

void MainWindow::refreshFilter() {
    QString regExp;
    if (ui->exportButton->isChecked()) {
        regExp.append(SymbolTableModel::symbolTypes[SymbolFilterExported] + "|");
    }
    if (ui->importButton->isChecked()) {
        regExp.append(SymbolTableModel::symbolTypes[SymbolFilterImported] + "|");
    }
    // remove last OR
    if (!regExp.isEmpty()) {
        regExp.remove(regExp.length() - 1, 1);
    }
    symbolFilterModel.setFilterRegExp("^"+regExp+"$");
}


