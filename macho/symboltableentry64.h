#ifndef SYMBOLTABLEENTRY64_H
#define SYMBOLTABLEENTRY64_H

#include "symboltableentry.h"

class SymbolTableEntry64 : public SymbolTableEntry
{
public:
    SymbolTableEntry64(MachOFile& file, struct nlist_64* entry, char* stringTable);
    virtual const char* getInternalName() const;
    virtual unsigned int getInternalType() const;
private:
    struct nlist_64* entry;
};

#endif // SYMBOLTABLEENTRY64_H
