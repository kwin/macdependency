#ifndef RPATHCOMMAND_H
#define RPATHCOMMAND_H

#include "MachO_global.h"
#include "loadcommand.h"

class MACHOSHARED_EXPORT RPathCommand : public LoadCommand
{
public:
    RPathCommand(MachOHeader* header);
    virtual ~RPathCommand();

    virtual unsigned int getSize() const;
    virtual unsigned int getStructureSize() const { return sizeof(command); }
    const char* getPath() const;
private:
    rpath_command command;
};

#endif // RPATHCOMMAND_H
