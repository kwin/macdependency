#include "machoheader.h"
#include "machofile.h"
#include "macho32header.h"
#include "macho64header.h"
#include "machoexception.h"
#include <mach/mach.h>

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
MachOHeader::MachOHeader(const MachOFile& file, bool reversedBO) :
        file(file, reversedBO), offset(this->file.getPosition())
{
}

MachOHeader::~MachOHeader() {

}

// taken over from dyld.cpp (V 97.1)
MachOHeader::CpuType MachOHeader::getHostCpuType() {
	struct host_basic_info info;
	mach_msg_type_number_t count = HOST_BASIC_INFO_COUNT;
	mach_port_t hostPort = mach_host_self();
	kern_return_t result = host_info(hostPort, HOST_BASIC_INFO, (host_info_t)&info, &count);
	mach_port_deallocate(mach_task_self(), hostPort);
	if (result != KERN_SUCCESS)
		throw "host_info() failed";
	//sHostCPUsubtype = info.cpu_subtype;
	return getCpuType(info.cpu_type);
}

MachOHeader::CpuType MachOHeader::getCpuType(unsigned int cpu) {
    MachOHeader::CpuType cpuType;
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
            cpuType = MachOHeader::CpuTypeOther;
    }
    return cpuType;
}

MachOHeader::FileType MachOHeader::getFileType() const {
    unsigned int fileType = getInternalFileType();
    if (fileType > NumFileTypes || fileType < 1) {
        return NumFileTypes;
    }
    return static_cast<FileType>(fileType-1);
}
