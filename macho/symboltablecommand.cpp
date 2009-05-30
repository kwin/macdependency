#include "symboltablecommand.h"
#include "symboltableentry32.h"
#include "symboltableentry64.h"

SymbolTableCommand::SymbolTableCommand(MachOHeader* header) :
        LoadCommand(header), symbols32(0), symbols64(0)
{
    file.readBytes((char*)&command, sizeof(command));

    unsigned int stringTableLength = file.getUint32(command.strsize);
    stringTable = new char[stringTableLength];

    // offset relative to beginning of architecture
    file.seek(header->getOffset() + file.getUint32(command.stroff));
    file.readBytes(stringTable, file.getUint32(command.strsize));

    unsigned int numberOfSymbols = file.getUint32(command.nsyms);
    // offset relative to beginning of architecture
    if (header->is64Bit()) {
        symbols64 = new struct nlist_64[numberOfSymbols];
        file.seek(header->getOffset() + file.getUint32(command.symoff));
        file.readBytes((char*)symbols64, sizeof(*symbols64) * numberOfSymbols);
        for (unsigned int n = 0; n < numberOfSymbols; n++) {
            SymbolTableEntry* entry = new SymbolTableEntry64(file, &symbols64[n], stringTable);
            symbolTableEntries.push_back(entry);
        }
    } else {
        symbols32 = new struct nlist[numberOfSymbols];
        file.seek(header->getOffset() + file.getUint32(command.symoff));
        file.readBytes((char*)symbols32, sizeof(*symbols32) * numberOfSymbols);
        for (unsigned int n = 0; n < numberOfSymbols; n++) {
            SymbolTableEntry* entry = new SymbolTableEntry32(file, &symbols32[n], stringTable);
            symbolTableEntries.push_back(entry);
        }
    }
}

SymbolTableCommand::~SymbolTableCommand()
{
    for (std::vector<SymbolTableEntry*>::iterator it = symbolTableEntries.begin();
    it != symbolTableEntries.end();
    ++it)
    {
        delete *it;
    }
    if (symbols32)
        delete[] symbols32;
    if (symbols64)
        delete[] symbols64;
    delete[] stringTable;
}

unsigned int SymbolTableCommand::getSize() {
    return file.getUint32(command.cmdsize);
}
