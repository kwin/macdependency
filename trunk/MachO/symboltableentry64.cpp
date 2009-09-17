#include "symboltableentry64.h"
#include "machofile.h"

SymbolTableEntry64::SymbolTableEntry64(MachOFile& file, struct nlist_64* entry, char* stringTable) :
        SymbolTableEntry(file, stringTable), entry(entry)
{
}

const char* SymbolTableEntry64::getInternalName() const {
    return &stringTable[file.getUint32(entry->n_un.n_strx)];
}

unsigned int SymbolTableEntry64::getInternalType() const {
    return entry->n_type;
}

