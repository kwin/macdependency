#ifndef LOADCOMMAND_H
#define LOADCOMMAND_H
#include "MachO_global.h"
#include "machoheader.h"
#include "/usr/include/mach-o/loader.h"
class MACHOSHARED_EXPORT LoadCommand
{
protected:
    LoadCommand(MachOHeader* header);
public:
    static LoadCommand* getLoadCommand(unsigned int cmd, MachOHeader* header);
    virtual ~LoadCommand();
    virtual unsigned int getSize() = 0;
protected:
    MachOHeader* header;
    MachOFile& file;
    unsigned int offset;
};

#endif // LOADCOMMAND_H
