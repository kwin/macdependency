#include "macho32header.h"
#include "machoexception.h"

MachO32Header::MachO32Header(const MachOFile& file, bool reversedBO) :
        MachOHeader(file, reversedBO)
{
    this->file.readBytes((char*)&header, sizeof(header));
}

MachOHeader::CpuType MachO32Header::getCpuType() const {
    return MachOHeader::getCpuType(file.getUint32(header.cputype));
}

unsigned int MachO32Header::getInternalFileType() const {
    return file.getUint32(header.filetype);
}

bool MachO32Header::is64Bit() const {
    return false;
}

unsigned int MachO32Header::getNumberOfLoadCommands() const {
    return file.getUint32(header.ncmds);
}

unsigned int MachO32Header::getLoadCommandSize() const {
    return file.getUint32(header.sizeofcmds);;
}

unsigned int MachO32Header::getSize() const {
    return sizeof(header);
}
