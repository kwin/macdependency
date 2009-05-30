#include "machoheader.h"

#include "macho32header.h"
#include "macho64header.h"
#include "machoexception.h"

MachOHeader* MachOHeader::getHeader(MachOFile& file, uint32_t magic) {
    MachOHeader* header;
    switch (magic) {
        case MH_MAGIC:
            header = new MachO32Header(file, false);
            break;
        case MH_CIGAM:
            header = new MachO32Header(file, true);
            break;
        case MH_MAGIC_64:
            header = new MachO64Header(file, false);
            break;
        case MH_CIGAM_64:
            header = new MachO64Header(file, true);
            break;
        default:
            throw MachOException("Invalid magic number. No Mach-O file.");
    }
    return header;
}

// for every header create a new filehandler with correct byte order
MachOHeader::MachOHeader(MachOFile& file, bool reversedBO) :
        file(file, reversedBO), offset(this->file.getPosition())
{
}

MachOHeader::~MachOHeader() {

}

MachOHeader::CpuType MachOHeader::getCpuType() {
     MachOHeader::CpuType cpuType;
    unsigned int cpu = getInternalCpuType();
    switch(cpu) {
        case CPU_TYPE_POWERPC:
            cpuType = MachOHeader::CpuTypePowerPc;
            break;
        case CPU_TYPE_I386:
            cpuType = MachOHeader::CpuTypeI386;
            break;
        case CPU_TYPE_POWERPC64:
            cpuType = MachOHeader::CpuTypePowerPc64;
            break;
        case CPU_TYPE_X86_64:
            cpuType = MachOHeader::CpuTypeX8664;
            break;
        default:
            throw MachOException("Invalid CPU type");

    }
    return cpuType;
}

MachOHeader::FileType MachOHeader::getFileType() {
    unsigned int fileType = getInternalFileType();
    if (fileType > NumFileTypes || fileType < 1) {
        throw MachOException("Invalid file type");
    }
    return static_cast<FileType>(fileType-1);
}

