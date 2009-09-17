#include "symboltableentry.h"
#include "macho.h"
#include "machofile.h"
#include "demangler.h"

SymbolTableEntry::SymbolTableEntry(MachOFile& file, char* stringTable) :
        file(file), stringTable(stringTable)
{
}

SymbolTableEntry::~SymbolTableEntry() {

}

string SymbolTableEntry::getName(bool shouldDemangle) const {
    const char* internalName = getInternalName();
    if (shouldDemangle) {
        return MachO::demangler->demangleName(internalName);
    }
    return internalName;
}

SymbolTableEntry::Type SymbolTableEntry::getType() const {
    unsigned int type = getInternalType();

    if (type & N_STAB) {
        return TypeDebug;
    }
    if (type & N_PEXT) {
        return TypePrivateExtern;
    }
    if (type & N_EXT) {
        if ((type & N_TYPE) == N_UNDF)
            return TypeImported;
        else
            return TypeExported;
    }
    else
        return TypeLocal;
}

/*
QDebug& operator<<(QDebug& dbg, const SymbolTableEntry& symbolTable) {
    unsigned int type = symbolTable.getInternalType();
    dbg.nospace() <<  "Name:" << symbolTable.getInternalName() << "Type" << (type & N_TYPE) << "STAB" << (type & N_STAB) << "PEXT" << (type & N_PEXT) << "NTYPE" << (type & N_TYPE) << "NEXT" << (type & N_EXT);
    return dbg.space();
}
*/
