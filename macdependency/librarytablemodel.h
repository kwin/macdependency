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
        enum State {
            Normal,
            Warning,
            Error
        };

        LibraryItem(LibraryItem* parent, DylibCommand* command, MachOArchitecture* architecture, MachO* file, State state = Normal) :
                parent(parent), dylibCommand(command), architecture(architecture), file(file), state(state), children(new std::vector<LibraryItem*>()) {
        }

        // copy constructor, which only takes over the children (with overriden parents)
        LibraryItem(const LibraryItem* item, LibraryItem* parent, DylibCommand* command) :
                parent(parent), dylibCommand(command), architecture(item->architecture), file(item->file), state(item->state), children(new std::vector<LibraryItem*>()) {
            copyChildren(item->children, this);
        }

        // copy constructor, which only overrides the parent (even those of the children, cause otherwise the parent chain would be inconsistent)
        LibraryItem(const LibraryItem* item, LibraryItem* parent) :
                parent(parent), dylibCommand(item->dylibCommand), architecture(item->architecture), file(item->file), state(item->state), children(new std::vector<LibraryItem*>()) {
            copyChildren(item->children, this);
        }

        ~LibraryItem() {
            for (std::vector<LibraryItem*>::iterator it = children->begin();
            it != children->end();
            ++it) {
                delete (*it);
            }
            delete children;
        }

    private:
        void copyChildren(std::vector<LibraryItem*>* children, LibraryItem* parent) {
            for (std::vector<LibraryItem*>::iterator it = children->begin();
            it != children->end();
            ++it) {
                this->children->push_back(new LibraryItem(*it, parent));
            }
        }

    public:
        LibraryItem* parent;
        DylibCommand* dylibCommand;
        MachOArchitecture* architecture;
        MachO* file;
        State state;
        std::vector<LibraryItem*>* children;
    };

private:
    enum {
        ColumnName = 0,
        ColumnType,
        ColumnCompatibleVersion,
        ColumnCurrentVersion,
        NumberOfColumns
    };
    const static QString columnLabels[NumberOfColumns];
    const static QString types[];

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
