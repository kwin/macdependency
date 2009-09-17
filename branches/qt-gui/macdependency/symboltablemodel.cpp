#include "symboltablemodel.h"

// static variables
const QString SymbolTableModel::columnLabels[SymbolTableModel::NumberOfColumns] = { tr("Type"), tr("Name") };
const QString SymbolTableModel::symbolTypes[SymbolTableEntry::NumTypes] = { tr("Export"), tr("Import"), tr("Local"), tr("Debug"), tr("Private Export") };

SymbolTableModel::SymbolTableModel(MachOArchitecture* architecture, bool shouldDemangleNames) :
        symbolTableEntries(0), shouldDemangleNames(shouldDemangleNames), shouldReleaseSymbolTableEntries(false)
{
    if (architecture != 0) {
        for (std::vector<LoadCommand*>::const_iterator it = architecture->getLoadCommandsBegin();
        it != architecture->getLoadCommandsEnd();
        ++it)
        {
            // check if it is dylibcommand
            SymbolTableCommand* command = dynamic_cast<SymbolTableCommand*> (*it);
            if (command != 0) {
                symbolTableEntries = (command->getSymbolTableEntries());
                break;
            }
        }
    }
    if (!symbolTableEntries) {
        symbolTableEntries = new std::vector<const SymbolTableEntry*>;
        shouldReleaseSymbolTableEntries = true;
    }
}

SymbolTableModel::~SymbolTableModel() {
    if (shouldReleaseSymbolTableEntries) {
        delete symbolTableEntries;
    }
}

int SymbolTableModel::rowCount(const QModelIndex &parent) const {
    if (parent.isValid())
        return 0;
    return symbolTableEntries->size();
}

int SymbolTableModel::columnCount (const QModelIndex & parent) const {
    if (parent.isValid())
        return 0;
    return NumberOfColumns;
}

QVariant SymbolTableModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid())
        return QVariant();

    if (index.row() < 0 || index.row() >= (int)symbolTableEntries->size())
        return QVariant();

    if (role == Qt::DisplayRole) {
        const SymbolTableEntry* symbol = symbolTableEntries->at(index.row());
        switch(index.column()) {
        case ColumnType:
            SymbolTableEntry::Type type = symbol->getType();
            return symbolTypes[type];
            break;
        case ColumnName:
            return symbol->getName(shouldDemangleNames);
            break;
        default:
            return QVariant();
        }
    }
    return QVariant();
}

QVariant SymbolTableModel::headerData(int section, Qt::Orientation orientation,
                                      int role) const {
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        if (section >= 0 && section < NumberOfColumns)
            return columnLabels[section];
    }
    return QVariant();
}
