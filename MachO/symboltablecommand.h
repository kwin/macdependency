#ifndef SYMBOLTABLECOMMAND_H
#define SYMBOLTABLECOMMAND_H

#include "loadcommand.h"
#include "/usr/include/mach-o/nlist.h"
#include <vector>
#include "symboltableentry.h"

class SymbolTableCommand : public LoadCommand
{
public:
    SymbolTableCommand(MachOHeader* header);
    virtual ~SymbolTableCommand();
    virtual unsigned int getSize() const;

    std::vector<const SymbolTableEntry*>::const_iterator getSymbolTableEntryBegin() const;
    std::vector<const SymbolTableEntry*>::const_iterator getSymbolTableEntryEnd() const;
    const std::vector<const SymbolTableEntry*>* getSymbolTableEntries() const;
private:
    symtab_command command;
    mutable struct nlist* symbols32;
    mutable struct nlist_64* symbols64;
    mutable char* stringTable;
    mutable std::vector<const SymbolTableEntry*> symbolTableEntries;

    void readSymbolTable() const;
};

#endif // SYMBOLTABLECOMMAND_H
