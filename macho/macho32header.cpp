#include "macho32header.h"
#include "machoexception.h"

MachO32Header::MachO32Header(MachOFile& file, bool reversedBO) :
        MachOHeader(file, reversedBO)
{
    this->file.readBytes((char*)&header, sizeof(header));
}

unsigned int MachO32Header::getInternalCpuType() {
    return file.getUint32(header.cputype);
}

unsigned int MachO32Header::getInternalFileType() {
    return file.getUint32(header.filetype);
}

bool MachO32Header::is64Bit() {
    return false;
}

unsigned int MachO32Header::getNumberOfLoadCommands() {
    return file.getUint32(header.ncmds);
}
