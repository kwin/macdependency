#include "rpathcommand.h"
#include "machofile.h"
#include "machoheader.h"

RpathCommand::RpathCommand(MachOHeader* header) :
        LoadCommand(header)
{
    file.readBytes((char*)&command, sizeof(command));
}

RpathCommand::~RpathCommand() {
}

unsigned int RpathCommand::getSize() const {
    return file.getUint32(command.cmdsize);
}

const char* RpathCommand::getPath() const {
    return getLcDataString(command.path.offset);
}
