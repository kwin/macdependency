#include "uuidcommand.h"
#include "machofile.h"
#include "machoheader.h"
#include <sstream>

UuidCommand::UuidCommand(MachOHeader* header) :
        LoadCommand(header)
{
    file.readBytes((char*)&command, sizeof(command));
}

UuidCommand::~UuidCommand() {
}

unsigned int UuidCommand::getSize() const {
    return file.getUint32(command.cmdsize);
}

const uint8_t* UuidCommand::getUuid() const {
    return command.uuid;
}

