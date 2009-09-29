#include "rpathcommand.h"
#include "machofile.h"
#include "machoheader.h"

RPathCommand::RPathCommand(MachOHeader* header) :
        LoadCommand(header)
{
    file.readBytes((char*)&command, sizeof(command));
}

RPathCommand::~RPathCommand() {
}

unsigned int RPathCommand::getSize() const {
    return file.getUint32(command.cmdsize);
}

const char* RPathCommand::getPath() const {
    return getLcDataString(command.path.offset);
}
