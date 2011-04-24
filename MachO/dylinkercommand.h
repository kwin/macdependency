#ifndef DYLINKERCOMMAND_H
#define DYLINKERCOMMAND_H

#include "macho_global.h"
#include "loadcommand.h"
class EXPORT DylinkerCommand : public LoadCommand
{
public:
    DylinkerCommand(MachOHeader* header);
    virtual ~DylinkerCommand();
    virtual unsigned int getSize() const;
    virtual unsigned int getStructureSize() const { return sizeof(command); }
    string getName() const;
private:
    dylinker_command command;

};

#endif // DYLINKERCOMMAND_H
