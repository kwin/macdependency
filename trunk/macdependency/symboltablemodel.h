#ifndef SYMBOLTABLEMODEL_H
#define SYMBOLTABLEMODEL_H

#include <QtCore/QAbstractTableModel>
#include <vector>
#include "symboltablecommand.h"
#include "machoarchitecture.h"

class SymbolTableModel : public QAbstractTableModel
{
public:
    static const QString symbolTypes[SymbolTableEntry::NumTypes];

    SymbolTableModel(MachOArchitecture* architecture, bool shouldDemangleNames);
    virtual ~SymbolTableModel();
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount ( const QModelIndex & parent = QModelIndex() ) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;
private:
    enum {
        ColumnType = 0,
        ColumnName,
        NumberOfColumns
    };
    static const QString columnLabels[];
    const std::vector<const SymbolTableEntry*>* symbolTableEntries;
    const bool shouldDemangleNames;
    bool shouldReleaseSymbolTableEntries;
};

#endif // SYMBOLTABLEMODEL_H
