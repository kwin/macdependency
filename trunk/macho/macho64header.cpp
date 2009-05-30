#include "macho64header.h"
#include "machoexception.h"

MachO64Header::MachO64Header(MachOFile& file, bool reversedBO) :
        MachOHeader(file, reversedBO)
{

    this->file.readBytes((char*)&header, sizeof(header));
}

unsigned int MachO64Header::getInternalCpuType() {
     return file.getUint32(header.cputype);
}

unsigned int MachO64Header::getInternalFileType() {
    return file.getUint32(header.filetype);
}

bool MachO64Header::is64Bit() {
    return true;
}

unsigned int MachO64Header::getNumberOfLoadCommands() {
    return file.getUint32(header.ncmds);
}
