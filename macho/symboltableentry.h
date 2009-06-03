#ifndef SYMBOLTABLEENTRY_H
#define SYMBOLTABLEENTRY_H

#include "/usr/include/mach-o/nlist.h"
#include "machofile.h"
#include <QtCore/QDebug>

class SymbolTableEntry
{
public:
    SymbolTableEntry(MachOFile& file, char* stringTable);
    virtual ~SymbolTableEntry();
    QString getName(bool shouldDemangle) const;

    enum Type {
        TypeExported = 0,
        TypeImported,
        TypeLocal,
        TypeDebug,
        TypePrivateExtern,
        NumTypes
    };

    Type getType() const;
    virtual unsigned int getInternalType() const = 0;
    virtual const char* getInternalName() const = 0;

protected:
    MachOFile& file;
    char* stringTable;
};


QDebug &operator<<(QDebug &dbg, const SymbolTableEntry &symbolTable);

#endif // SYMBOLTABLEENTRY_H
