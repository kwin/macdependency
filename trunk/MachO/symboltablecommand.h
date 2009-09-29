#ifndef SYMBOLTABLECOMMAND_H
#define SYMBOLTABLECOMMAND_H

#include "loadcommand.h"
#include <mach-o/nlist.h>
#include <list>

class SymbolTableEntry;
class EXPORT SymbolTableCommand : public LoadCommand
{
private:
	typedef list<const SymbolTableEntry*> SymbolTableEntries;
	typedef SymbolTableEntries::iterator SymbolTableEntriesIterator;
public:
	typedef SymbolTableEntries::const_iterator SymbolTableEntriesConstIterator;
	
    SymbolTableCommand(MachOHeader* header);
    virtual ~SymbolTableCommand();
    virtual unsigned int getSize() const;

    SymbolTableEntriesConstIterator getSymbolTableEntryBegin() const;
    SymbolTableEntriesConstIterator getSymbolTableEntryEnd() const;
    //const std::vector<const SymbolTableEntry*>* getSymbolTableEntries() const;
private:
    symtab_command command;
    mutable struct nlist* symbols32;
    mutable struct nlist_64* symbols64;
    mutable char* stringTable;
    mutable SymbolTableEntries symbolTableEntries;

    void readSymbolTable() const;
};

#endif // SYMBOLTABLECOMMAND_H
