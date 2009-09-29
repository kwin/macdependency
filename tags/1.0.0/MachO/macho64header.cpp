#include "macho64header.h"
#include "machoexception.h"

MachO64Header::MachO64Header(const MachOFile& file, bool reversedBO) :
        MachOHeader(file, reversedBO)
{

    this->file.readBytes((char*)&header, sizeof(header));
}

MachOHeader::CpuType MachO64Header::getCpuType() const {
	return MachOHeader::getCpuType(file.getUint32(header.cputype));
}

unsigned int MachO64Header::getInternalFileType() const {
    return file.getUint32(header.filetype);
}

bool MachO64Header::is64Bit() const {
    return true;
}

unsigned int MachO64Header::getNumberOfLoadCommands() const {
    return file.getUint32(header.ncmds);
}

unsigned int MachO64Header::getLoadCommandSize() const {
    return file.getUint32(header.sizeofcmds);
}

unsigned int MachO64Header::getSize() const {
    return sizeof(header);
}
