#include "librarytablemodel.h"
#include "macho.h"
#include "machoexception.h"

#include <QtCore/QDateTime>

// static variables
const QString LibraryTableModel::columnLabels[LibraryTableModel::NumberOfColumns] = { tr("Name"), tr("Compatible Version"), tr("Current Version")};

LibraryTableModel::LibraryTableModel(MachOArchitecture* architecture, MachO* file, QTextBrowser* logBrowser, QTextBrowser* loadedLibrariesBrowser) : logBrowser(logBrowser), loadedLibrariesBrowser(loadedLibrariesBrowser)
{
    QTime timer;
    timer.start();
    root = new LibraryItem(0, 0, architecture, file);
    createChildItems(root);
    loadedLibrariesBrowser->append(QString(tr("%1 libraries loaded in %2 ms")).arg(itemCache.size()).arg(timer.elapsed()));
}

LibraryTableModel::LibraryItem* LibraryTableModel::createLibraryItem(DylibCommand* dylibCommand, LibraryItem* parent) {
    QString libraryName = dylibCommand->getResolvedName(root->file->getDirectory());
    LibraryItem* item = 0;
    // check if library is already loaded (only load each library once)
    std::map<QString,LibraryItem*>::iterator it = itemCache.find(libraryName);

    if (it == itemCache.end()) {
        try {
            QTime timer;
            timer.start();
            MachO* library = new MachO(libraryName);
            loadedLibrariesBrowser->append(QString(tr("%1 loaded in %2 ms").arg(libraryName).arg(timer.elapsed())));

            MachOArchitecture* architecture = library->getCompatibleArchitecture(parent->architecture);
            if (architecture == NULL) {
                logBrowser->append(QString(tr("Couldn't find correct architecture in %1")).arg(libraryName));
                return 0;
            }

            // for each child create also a library item
            item = new LibraryItem(parent, dylibCommand, architecture, library);
            if (item != 0) {
                createChildItems(item);
                //logBrowser->append(QString(tr("Put library %1 in cache")).arg(libraryName));
                itemCache.insert(std::pair<QString,LibraryItem*>(libraryName,item));
            }

        } catch (MachOException& exception) {
            logBrowser->append(QString(tr("Couldn't load library %1: %2")).arg(libraryName).arg(exception.getCause()));
            return 0;
        }


    } else {
        LibraryItem* cachedItem = it->second;
        item = new LibraryItem(cachedItem, parent);
    }

    // check version information (only compatible version information is relevant)
    if (item->architecture->getDynamicLibIdCommand() != 0) {
        int currentVersion = item->architecture->getDynamicLibIdCommand()->getCompatibleVersion();
        int requestedVersion = item->architecture->getDynamicLibIdCommand()->getCompatibleVersion();

        if (currentVersion != 0 && requestedVersion != 0 && currentVersion < requestedVersion) {
            logBrowser->append(QString(tr("Library %1 was requested in version %2, but only exists in version %3"))
                               .arg(libraryName)
                               .arg(DylibCommand::getVersionString(requestedVersion))
                               .arg(DylibCommand::getVersionString(currentVersion)));
        }
    }
    return item;
}

void LibraryTableModel::createChildItems(LibraryItem* parent) {

    for (std::vector<LoadCommand*>::iterator it = parent->architecture->getLoadCommandsBegin();
    it != parent->architecture->getLoadCommandsEnd();
    ++it)
    {
        LoadCommand* command = (*it);
        // check if it is dylibcommand
        DylibCommand* dylibCommand = dynamic_cast<DylibCommand*> (command);
        if (dylibCommand != NULL && !dylibCommand->isId()) {
            LibraryItem* item = createLibraryItem(dylibCommand, parent);
            if (item != 0)
                parent->children->push_back(item);
        }
    }

}

LibraryTableModel::~LibraryTableModel() {
    // delete all files of opened libraries
    for (std::map<QString, LibraryItem*>::iterator it = itemCache.begin();
    it != itemCache.end();
    ++it) {
        delete it->second->file;

    }

    // deleting root is enough, since root recursively deletes all children
    delete root;


}

int LibraryTableModel::rowCount(const QModelIndex &parent) const {
    if (!parent.isValid())
        return 1;
    if (parent.column() > 0)
        return 0;
    LibraryItem* item = getModelIndexData(parent);
    return item->children->size();
}

int LibraryTableModel::columnCount(const QModelIndex & /*parent*/) const {
    return NumberOfColumns;
}

QVariant LibraryTableModel::data(const QModelIndex &index, int role) const {
    // index contains the libraryitem
    LibraryItem* item = getModelIndexData(index);
    if (item == 0)
        return QVariant();

    if (role == Qt::DisplayRole || role == SortRole) {
        DylibCommand* dylib = item->dylibCommand;
        switch(index.column()) {
        case ColumnName:
            if (dylib)
                return dylib->getName();
            else
                return item->file->getFileName();
            break;
        case ColumnCurrentVersion:
            if (dylib) {
                // we need a different type for sorting versions
                if (role == SortRole) {
                    return dylib->getCurrentVersion();
                } else {
                    return dylib->getVersionString(dylib->getCurrentVersion());
                }
            }
            break;
        case ColumnCompatibleVersion:
            if (dylib) {
                // we need a different type for sorting versions
                if (role == SortRole) {
                    return dylib->getCompatibleVersion();
                } else {
                    return dylib->getVersionString(dylib->getCompatibleVersion());
                }
            }
            break;
        default:
            return QVariant();
        }
    } else if (role == Qt::DecorationRole && index.column() == 0) {
        return item->file->getIcon();
    }
    return QVariant();
}



QVariant LibraryTableModel::headerData(int section, Qt::Orientation orientation,
                                       int role) const {
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        if (section >= 0 && section < NumberOfColumns)
            return columnLabels[section];
        else
            return QVariant();
    }
    return QVariant();
}

QModelIndex LibraryTableModel::index(int row, int column, const QModelIndex &parent) const
{
    if (column < 0 || column >= NumberOfColumns || row < 0)
        return QModelIndex();

    // if parent is invalid, create root
    LibraryItem* parentItem = getModelIndexData(parent);
    if (parentItem == 0) {
        return createIndex(row, column, root);
    }
    if (row >= parentItem->children->size() || parentItem->children->size() == 0)
        return QModelIndex();
    return createIndex(row, column, parentItem->children->at(row));
}

QModelIndex LibraryTableModel::parent(const QModelIndex& child) const {
    if (!child.isValid())
        return QModelIndex();

    // is child already the root?
    LibraryItem* childItem = getModelIndexData(child);
    if (childItem->parent == 0)
        return QModelIndex();

    // is parent the root (i.e. it has no parent itself)?
    if (childItem->parent->parent == 0)
        return createIndex(0,0, root);

    // get the correct row where the parent is located (look in children of parent's parent)
    int row = 0;
    for (std::vector<LibraryItem*>::iterator it = childItem->parent->parent->children->begin();
    it !=  childItem->parent->parent->children->end();
    ++it) {
        if ((*it) == childItem->parent)
            // parent column is always zero, cause only the first column has children
            return createIndex(row, 0, childItem->parent);
        row++;
    }
    return QModelIndex();
}

