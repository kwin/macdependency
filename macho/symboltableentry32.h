#ifndef SYMBOLTABLEENTRY32_H
#define SYMBOLTABLEENTRY32_H

#include "symboltableentry.h"

class SymbolTableEntry32 : public SymbolTableEntry
{
public:
    SymbolTableEntry32(MachOFile& file, struct nlist* entry, char* stringTable);
    virtual const char* getInternalName() const;
    virtual unsigned int getInternalType() const;
private:
    struct nlist* entry;
};


#endif // SYMBOLTABLEENTRY32_H
