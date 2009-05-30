#ifndef MACHOHEADER_H
#define MACHOHEADER_H

#include "MachO_global.h"
#include "machofile.h"
#include "/usr/include/mach-o/loader.h"

class MACHOSHARED_EXPORT MachOHeader
{
public:
    static MachOHeader* getHeader(MachOFile& file, uint32_t magic);
    virtual ~MachOHeader();

    enum CpuType {
        CpuTypePowerPc,
        CpuTypeI386,
        CpuTypePowerPc64,
        CpuTypeX8664,
        NumCpuTypes
    };

    enum FileType {

        FileTypeObject,		/* relocatable object file */
        FileTypeExecute,    		/* demand paged executable file */
        FileTypeVmLib,  		/* fixed VM shared library file */
        FileTypeCore,   		/* core file */
        FileTypePreload,                /* preloaded executable file */
        FileTypeDylib,          	/* dynamically bound shared library */
        FileTypeDylinker,               /* dynamic link editor */
        FileTypeBundle,                 /* dynamically bound bundle file */
        FileTypeDylibStub,              /* shared library stub for static linking only, no section contents */
        FileTypeDsym,   		/* companion file with only debug sections */
        NumFileTypes
    };
    FileType getFileType();
    CpuType getCpuType();
    virtual unsigned int getNumberOfLoadCommands() = 0;
    virtual bool is64Bit() = 0;
    MachOFile& getFile() { return file;}
    unsigned int getOffset() { return offset; }

protected:
    MachOHeader(MachOFile& file, bool reversedBO);
    MachOFile file;
    unsigned int offset;

    virtual unsigned int getInternalCpuType() = 0;
    virtual unsigned int getInternalFileType() = 0;
};

#endif // MACHOHEADER_H
