#include "genericcommand.h"

GenericCommand::GenericCommand(MachOHeader* header) :
        LoadCommand(header)
{
    file.readBytes((char*)&command, sizeof(command));
}

unsigned int GenericCommand::getSize() {
    return file.getUint32(command.cmdsize);
}
