#include "symboltableentry32.h"
#include "machofile.h"

SymbolTableEntry32::SymbolTableEntry32(MachOFile& file, struct nlist* entry, char* stringTable) :
        SymbolTableEntry(file, stringTable), entry(entry)
{
}

const char* SymbolTableEntry32::getInternalName() const {
    return &stringTable[file.getUint32(entry->n_un.n_strx)];
}

unsigned int SymbolTableEntry32::getInternalType() const {
    return entry->n_type;
}


