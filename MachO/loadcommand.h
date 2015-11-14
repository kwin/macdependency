#ifndef LOADCOMMAND_H
#define LOADCOMMAND_H
#include "MachO_global.h"
#include "machoheader.h"
#include "/usr/include/mach-o/loader.h"
class MACHOSHARED_EXPORT LoadCommand
{
protected:
    LoadCommand(MachOHeader* header);
public:
    static LoadCommand* getLoadCommand(unsigned int cmd, MachOHeader* header);
    virtual ~LoadCommand();
    virtual unsigned int getSize() const = 0;
    // is smaller than getSize() because it only returns the size of the command structure
    // only necessary if command uses getLcData()
    virtual unsigned int getStructureSize() const { return 0; }
protected:
    MachOHeader* header;
    MachOFile& file;
    const unsigned int offset;

    const char* getLcDataString(unsigned int offset) const;
private:
    mutable char* lcData;

    void readLcData() const;
};

#endif // LOADCOMMAND_H
