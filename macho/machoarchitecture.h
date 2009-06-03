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
    MachOArchitecture(MachOFile& file, uint32_t magic, unsigned int size);
    ~MachOArchitecture();

    const MachOHeader* getHeader() { return header; }
    std::vector<LoadCommand*>::const_iterator getLoadCommandsBegin() const { if(!hasReadLoadCommands) { readLoadCommands(); } return loadCommands.begin(); }
    std::vector<LoadCommand*>::const_iterator getLoadCommandsEnd() const { if(!hasReadLoadCommands) { readLoadCommands(); } return loadCommands.end(); }
    DylibCommand* getDynamicLibIdCommand() const { if(!hasReadLoadCommands) { readLoadCommands(); } return dynamicLibIdCommand; }
    unsigned int getSize() const;

private:
    MachOHeader* header;
    MachOFile& file;
    const unsigned int size;
    mutable bool hasReadLoadCommands;
    void readLoadCommands() const;

    mutable std::vector<LoadCommand*> loadCommands;
    mutable DylibCommand* dynamicLibIdCommand;
    mutable std::vector<const char*> rPaths;
};

#endif // MACHOARCHITECTURE_H
