#ifndef MACHO32HEADER_H
#define MACHO32HEADER_H


#include "machoheader.h"

class MachO32Header : public MachOHeader
{
public:
    MachO32Header(const MachOFile& file, bool reversedBO);
    virtual unsigned int getNumberOfLoadCommands() const;
    virtual bool is64Bit() const;
    virtual unsigned int getLoadCommandSize() const;    // size of load command following the header
    virtual unsigned int getSize() const;   // size of header only
    virtual MachOHeader::CpuType getCpuType() const;
protected:
    virtual unsigned int getInternalFileType() const;
private:
    mach_header header;
};

#endif // MACHO32HEADER_H
