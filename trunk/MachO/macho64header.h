#ifndef MACHO64HEADER_H
#define MACHO64HEADER_H

#include "machoheader.h"

class MachO64Header : public MachOHeader
{
public:
    MachO64Header(const MachOFile& file, bool reversedBO);
    virtual unsigned int getNumberOfLoadCommands() const;
    virtual bool is64Bit() const;
    virtual unsigned int getLoadCommandSize() const;    // size of load command following the header
    virtual unsigned int getSize() const;   // size of header only
	virtual MachOHeader::CpuType getCpuType() const;
protected:
    virtual unsigned int getInternalFileType() const;
private:
    mach_header_64 header;
};

#endif // MACHO64HEADER_H
