#ifndef RPATHCOMMAND_H
#define RPATHCOMMAND_H

#include "MachO_global.h"
#include "loadcommand.h"

class EXPORT RpathCommand : public LoadCommand
{
public:
    RpathCommand(MachOHeader* header);
    virtual ~RpathCommand();

    virtual unsigned int getSize() const;
    virtual unsigned int getStructureSize() const { return sizeof(command); }
    const char* getPath() const;
private:
    rpath_command command;
};

#endif // RPATHCOMMAND_H
