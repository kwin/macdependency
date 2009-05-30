#include "machoarchitecture.h"
#include "machoexception.h"



MachOArchitecture::MachOArchitecture(MachOFile& file, uint32_t magic) :
    header(MachOHeader::getHeader(file, magic)), file(header->getFile()), hasReadLoadCommands(false), dynamicLibIdCommand(0)
{
}

void MachOArchitecture::readLoadCommands() {
    // read out number of commands
    unsigned int numberOfCommands = header->getNumberOfLoadCommands();
    // read out command identifiers
    for (unsigned int n=0; n<numberOfCommands; n++) {
        unsigned int commandOffset = file.getPosition();
        unsigned int cmd = file.readUint32();

        file.seek(commandOffset);
        LoadCommand* loadCommand = LoadCommand::getLoadCommand(cmd, header);
        DylibCommand* dylibCommand = dynamic_cast<DylibCommand*>(loadCommand);
        if (dylibCommand != 0 && dylibCommand->isId()) {
            dynamicLibIdCommand = dylibCommand;
        }
        loadCommands.push_back(loadCommand);
        file.seek(commandOffset + loadCommand->getSize());
    }
    hasReadLoadCommands = true;
}

MachOArchitecture::~MachOArchitecture() {
    delete header;

    for (std::vector<LoadCommand*>::iterator it = loadCommands.begin();
    it != loadCommands.end();
    ++it)
    {
        delete *it;
    }
}



