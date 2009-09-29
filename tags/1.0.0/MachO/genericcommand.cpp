#include "genericcommand.h"
#include "machofile.h"
#include "machoheader.h"

GenericCommand::GenericCommand(MachOHeader* header) :
        LoadCommand(header)
{
    file.readBytes((char*)&command, sizeof(command));
}

unsigned int GenericCommand::getSize() const {
    return file.getUint32(command.cmdsize);
}
