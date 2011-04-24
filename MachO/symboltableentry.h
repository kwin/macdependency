#ifndef SYMBOLTABLEENTRY_H
#define SYMBOLTABLEENTRY_H

#include "macho_global.h"
#include <mach-o/nlist.h>
class MachOFile;

class EXPORT SymbolTableEntry
{
public:
    SymbolTableEntry(MachOFile& file, char* stringTable);
    virtual ~SymbolTableEntry();
    string getName(bool shouldDemangle) const;

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

#endif // SYMBOLTABLEENTRY_H
