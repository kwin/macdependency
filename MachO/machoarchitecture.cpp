#include "machoarchitecture.h"
#include "machofile.h"
#include "machoheader.h"
#include "loadcommand.h"
#include "dylibcommand.h"
#include "machoexception.h"
#include "rpathcommand.h"
#include "macho.h"
#include "dynamicloader.h"


MachOArchitecture::MachOArchitecture(MachOFile& file, uint32_t magic, unsigned int size) :
    header(MachOHeader::getHeader(file, magic)), file(header->getFile()), size(size), hasReadLoadCommands(false), parent(0), dynamicLibIdCommand(0)
{
}

void MachOArchitecture::initParentArchitecture(const MachOArchitecture* parent) {
    this->parent = parent;
}

string MachOArchitecture::getResolvedName(const string& name, const string& workingPath) const {
    string absoluteFileName = MachO::dynamicLoader->getPathname(name, this, workingPath);
    if (!absoluteFileName.empty())
        return absoluteFileName;
    // return unresolved name if it cannot be resolved to a valid absolute name
    return name;
}

std::vector<string*> MachOArchitecture::getRPaths() const {
    // try to get it from the parent (recursively)
    std::vector<string*> prevRPaths = parent?(std::vector<string*>(parent->getRPaths())):(std::vector<string*>());
    // add own rpaths to the end
    prevRPaths.insert(prevRPaths.end(), rPaths.begin(), rPaths.end());
    return prevRPaths;
}

void MachOArchitecture::readLoadCommands() const {
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

        RPathCommand* rPathCommand = dynamic_cast<RPathCommand*>(loadCommand);
        if (rPathCommand != 0) {
            // try to replace placeholder
            string resolvedRPath = MachO::dynamicLoader->replacePlaceholder(rPathCommand->getPath(), this);
            if (resolvedRPath.empty()) {
                resolvedRPath = rPathCommand->getPath();
            }
            rPaths.push_back(new string(resolvedRPath));
        }
        loadCommands.push_back(loadCommand);
        file.seek(commandOffset + loadCommand->getSize());
    }
    hasReadLoadCommands = true;
}

MachOArchitecture::~MachOArchitecture() {
    delete header;

    for (LoadCommandsIterator it = loadCommands.begin();
    it != loadCommands.end();
    ++it)
    {
        delete *it;
    }

    for (std::vector<string*>::iterator it2 = rPaths.begin();
    it2 != rPaths.end();
    ++it2)
    {
        delete *it2;
    }
}

unsigned int MachOArchitecture::getSize() const {
    return size;
}



