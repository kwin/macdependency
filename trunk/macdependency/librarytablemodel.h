#ifndef LIBRARYTABLEMODEL_H
#define LIBRARYTABLEMODEL_H


#include "macho.h"
#include "dylibcommand.h"
#include <vector>
#include <map>

#include <QtGui/QTextBrowser>
#include <QtCore/QAbstractItemModel>
#include <QtCore/QLinkedList>
#include <QtCore/QHash>

class LibraryTableModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    LibraryTableModel(MachOArchitecture* architecture, MachO* file, QTextBrowser* logBrowser, QTextBrowser* loadedLibrariesBrowser);
    ~LibraryTableModel();

    virtual bool hasChildren (const QModelIndex & parent = QModelIndex()) const;
    virtual bool canFetchMore (const QModelIndex & parent) const;
    virtual void fetchMore (const QModelIndex & parent);
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual int columnCount (const QModelIndex & parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;
    virtual QModelIndex index(int row, int column, const QModelIndex &parent) const;
    virtual QModelIndex parent(const QModelIndex& child) const;


    class LibraryItem {
    public:
        enum State {
            Normal,
            Warning,
            Error
        };

        LibraryItem(LibraryItem* parent, DylibCommand* command, MachOArchitecture* architecture, MachO* file, State state = Normal) :
                parent(parent), dylibCommand(command), architecture(architecture), file(file), state(state), children(new QList<LibraryItem*>()), depth(0) {
            if (parent) {
                depth = parent->depth +1;
            }
            numberOfDependencies = 1;
        }

        // copy constructor, which only takes over the children (with overriden parents)
        LibraryItem(const LibraryItem* item, LibraryItem* parent, DylibCommand* command) :
                parent(parent), dylibCommand(command), architecture(item->architecture), file(item->file), state(item->state), children(new QList<LibraryItem*>()), depth(0) {
            if (parent) {
                depth = parent->depth +1;
            }
            numberOfDependencies = 1;
            copyChildren(item);
        }

        // copy constructor, which only overrides the parent (even those of the children, cause otherwise the parent chain would be inconsistent)
        LibraryItem(const LibraryItem* item, LibraryItem* parent) :
                parent(parent), dylibCommand(item->dylibCommand), architecture(item->architecture), file(item->file), state(item->state), children(0), depth(0) {
            if (parent) {
                depth = parent->depth +1;
            }
            numberOfDependencies = 1;
            // don't do children yet
        }

        ~LibraryItem() {
            if (children) {
                while (!children->isEmpty()) {
                    delete children->takeFirst();
                }
                delete children;
            }
        }

        void copyChildren(const LibraryItem* source) {
            QList<LibraryItem*>::const_iterator it;
            for (it = source->children->constBegin(); it != source->children->constEnd(); ++it) {
                LibraryItem* child = new LibraryItem(*it, this);
                this->children->append(child);
                numberOfDependencies += child->numberOfDependencies;
            }
        }

    public:
        LibraryItem* parent;
        DylibCommand* dylibCommand;
        MachOArchitecture* architecture;
        MachO* file;
        State state;
        QList<LibraryItem*>* children; // if this is null, it is unknown whether there are children or not
        unsigned int numberOfDependencies;
        unsigned char depth;
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
    QHash<QString, LibraryItem*> itemCache;
    LibraryItem* root;

    LibraryItem* createLibraryItem(DylibCommand* dylibCommand, LibraryItem* parent);
    void createChildItems(LibraryItem* parent);
public:
    LibraryItem* getModelIndexData(const QModelIndex& index) const{
        if (!index.isValid())
            return 0;
        return static_cast<LibraryItem*>(index.internalPointer());
    }

    const static int SortRole = Qt::UserRole;

};

#endif // LIBRARYTABLEMODEL_H
