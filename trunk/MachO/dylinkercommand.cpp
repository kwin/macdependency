#include "dylinkercommand.h"
#include "machofile.h"
#include "machoheader.h"
#include <sstream>

DylinkerCommand::DylinkerCommand(MachOHeader* header) :
        LoadCommand(header) {
    file.readBytes((char*)&command, sizeof(command));
}

DylinkerCommand::~DylinkerCommand() {
}

unsigned int DylinkerCommand::getSize() const {
    return file.getUint32(command.cmdsize);
}

string DylinkerCommand::getName() const {
    return string(getLcDataString(command.name.offset));
}

