#ifndef MACHOARCHITECTURE_H
#define MACHOARCHITECTURE_H

#include "MachO_global.h"
#include "machofile.h"
#include "machoheader.h"
#include "loadcommand.h"
#include "dylibcommand.h"
#include <vector>

class MACHOSHARED_EXPORT MachOArchitecture
{
public:
    MachOArchitecture(MachOFile& file, uint32_t magic);
    ~MachOArchitecture();

    MachOHeader* getHeader() { return header; }
    std::vector<LoadCommand*>::iterator getLoadCommandsBegin() { if(!hasReadLoadCommands) { readLoadCommands(); } return loadCommands.begin(); }
    std::vector<LoadCommand*>::iterator getLoadCommandsEnd() { if(!hasReadLoadCommands) { readLoadCommands(); } return loadCommands.end(); }
    DylibCommand* getDynamicLibIdCommand() { if(!hasReadLoadCommands) { readLoadCommands(); } return dynamicLibIdCommand; }


private:

    MachOHeader* header;
    MachOFile& file;
    bool hasReadLoadCommands;
    void readLoadCommands();

    std::vector<LoadCommand*> loadCommands;
    DylibCommand* dynamicLibIdCommand;
};

#endif // MACHOARCHITECTURE_H
