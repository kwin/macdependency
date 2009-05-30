#ifndef GENERICCOMMAND_H
#define GENERICCOMMAND_H

#include "loadcommand.h"

class GenericCommand : public LoadCommand
{
public:
    GenericCommand(MachOHeader* header);
    virtual unsigned int getSize();
private:
    load_command command;
};

#endif // GENERICCOMMAND_H
