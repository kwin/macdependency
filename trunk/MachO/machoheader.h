#ifndef MACHOHEADER_H
#define MACHOHEADER_H

#include "MachO_global.h"
#include "machofile.h"
#include "/usr/include/mach-o/loader.h"

class EXPORT MachOHeader
{
public:
    static MachOHeader* getHeader(MachOFile& file, uint32_t magic);
    virtual ~MachOHeader();

    enum CpuType {
        CpuTypePowerPc,
        CpuTypeI386,
        CpuTypePowerPc64,
        CpuTypeX8664,
        CpuTypeOther,
        NumCpuTypes
    };

    enum FileType {

        FileTypeObject,		/* relocatable object file */
        FileTypeExecutable,    		/* demand paged executable file */
        FileTypeVmLib,  		/* fixed VM shared library file */
        FileTypeCore,   		/* core file */
        FileTypePreload,                /* preloaded executable file */
        FileTypeDylib,          	/* dynamically bound shared library */
        FileTypeDylinker,               /* dynamic link editor */
        FileTypeBundle,                 /* dynamically bound bundle file */
        FileTypeDylibStub,              /* shared library stub for static linking only, no section contents */
        FileTypeDsym,   		/* companion file with only debug sections */
		FileTypeKextBundle,   /* x86 64 kext bundle */
        NumFileTypes		/* stands also for unknown types */
    };
    FileType getFileType() const;
	virtual CpuType getCpuType() const = 0;
	static CpuType getHostCpuType();
    static CpuType getCpuType(unsigned int internalCpuType);
    virtual unsigned int getNumberOfLoadCommands() const = 0;
    virtual bool is64Bit() const = 0;
    MachOFile& getFile() { return file;}
    unsigned int getOffset() const { return offset; }
    virtual unsigned int getLoadCommandSize() const = 0;    // size of load command following the header
    virtual unsigned int getSize() const = 0;   // size of header only

protected:
    MachOHeader(const MachOFile& file, bool reversedBO);
    MachOFile file;
    const unsigned int offset;
    virtual unsigned int getInternalFileType() const = 0;
};

#endif // MACHOHEADER_H
