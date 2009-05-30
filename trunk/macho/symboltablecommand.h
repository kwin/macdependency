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
    ~SymbolTableCommand();
    virtual unsigned int getSize();

    std::vector<SymbolTableEntry*>::iterator getSymbolTableEntryBegin() { return symbolTableEntries.begin(); };
    std::vector<SymbolTableEntry*>::iterator getSymbolTableEntryEnd() { return symbolTableEntries.end(); };
    std::vector<SymbolTableEntry*>* getSymbolTableEntries() { return &symbolTableEntries; };
private:
    symtab_command command;
    struct nlist* symbols32;
    struct nlist_64* symbols64;
    char* stringTable;
    std::vector<SymbolTableEntry*> symbolTableEntries;
};

#endif // SYMBOLTABLECOMMAND_H
