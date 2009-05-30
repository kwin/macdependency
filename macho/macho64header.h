#ifndef MACHO64HEADER_H
#define MACHO64HEADER_H

#include "machoheader.h"

class MachO64Header : public MachOHeader
{
public:
    MachO64Header(MachOFile& file, bool reversedBO);
    virtual unsigned int getNumberOfLoadCommands();
    virtual bool is64Bit();
protected:
    virtual unsigned int getInternalCpuType();
    virtual unsigned int getInternalFileType();
private:
    mach_header_64 header;
};

#endif // MACHO64HEADER_H
