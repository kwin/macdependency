#include "macho32header.h"
#include "machoexception.h"

MachO32Header::MachO32Header(const MachOFile& file, bool reversedBO) :
        MachOHeader(file, reversedBO)
{
    this->file.readBytes((char*)&header, sizeof(header));
}

unsigned int MachO32Header::getInternalCpuType() const {
    return file.getUint32(header.cputype);
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
    unsigned int test = file.getUint32(header.sizeofcmds);
    return test;
}

unsigned int MachO32Header::getSize() const {
    return sizeof(header);
}
