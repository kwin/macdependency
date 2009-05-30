#ifndef MACHO32HEADER_H
#define MACHO32HEADER_H


#include "machoheader.h"

class MachO32Header : public MachOHeader
{
public:
    MachO32Header(MachOFile& file, bool reversedBO);
    virtual unsigned int getNumberOfLoadCommands();
    virtual bool is64Bit();
protected:
    virtual unsigned int getInternalCpuType();
    virtual unsigned int getInternalFileType();
private:
    mach_header header;
};

#endif // MACHO32HEADER_H
