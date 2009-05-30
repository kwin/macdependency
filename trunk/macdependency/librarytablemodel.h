#ifndef LIBRARYTABLEMODEL_H
#define LIBRARYTABLEMODEL_H


#include "macho.h"
#include "dylibcommand.h"
#include <vector>
#include <map>

#include <QtGui/QTextBrowser>
#include <QtCore/QAbstractItemModel>

class LibraryTableModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    LibraryTableModel(MachOArchitecture* architecture, MachO* file, QTextBrowser* logBrowser, QTextBrowser* loadedLibrariesBrowser);
    ~LibraryTableModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount (const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation,
                         int role = Qt::DisplayRole) const;
    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    QModelIndex parent(const QModelIndex& child) const;


    class LibraryItem {
        public:
            LibraryItem(LibraryItem* parent, DylibCommand* command, MachOArchitecture* architecture, MachO* file) :
                    parent(parent), dylibCommand(command), architecture(architecture), file(file), children(new std::vector<LibraryItem*>()) {
            }

            // standard copy constructor with overridden parent
            LibraryItem(const LibraryItem* item, LibraryItem* parent) :
                parent(parent), dylibCommand(item->dylibCommand), architecture(item->architecture), file(item->file), children(new std::vector<LibraryItem*>()) {
                for (std::vector<LibraryItem*>::iterator it = item->children->begin();
                    it != item->children->end();
                    ++it) {
                        children->push_back(new LibraryItem(*it, this));
                }
            }

            ~LibraryItem() {
                for (std::vector<LibraryItem*>::iterator it = children->begin();
                    it != children->end();
                    ++it) {
                        delete (*it);
                }
                delete children;
            }


            LibraryItem* parent;
            DylibCommand* dylibCommand;
            MachOArchitecture* architecture;
            MachO* file;
            std::vector<LibraryItem*>* children;
    };

private:
    enum {
        ColumnName = 0,
        ColumnCompatibleVersion,
        ColumnCurrentVersion,
        NumberOfColumns
    };
    const static QString columnLabels[];

    QTextBrowser* logBrowser;
    QTextBrowser* loadedLibrariesBrowser;
    std::map<QString, LibraryItem*> itemCache;
    LibraryItem* root;

    LibraryItem* createLibraryItem(DylibCommand* dylibCommand, LibraryItem* parent);
    void createChildItems(LibraryItem* parent);
public:
    LibraryItem* getModelIndexData(const QModelIndex& index) const{
        if (!index.isValid())
            return NULL;
        return static_cast<LibraryItem*>(index.internalPointer());
    }

    const static int SortRole = Qt::UserRole;

};

#endif // LIBRARYTABLEMODEL_H
