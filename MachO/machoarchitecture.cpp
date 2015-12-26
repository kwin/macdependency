#include "machoarchitecture.h"
#include "machoexception.h"
#include "rpathcommand.h"
#include "macho.h"


MachOArchitecture::MachOArchitecture(MachOFile& file, uint32_t magic, unsigned int size) :
    header(MachOHeader::getHeader(file, magic)), file(header->getFile()), size(size), hasReadLoadCommands(false), parent(0), dynamicLibIdCommand(0)
{
}

void MachOArchitecture::initParentArchitecture(const MachOArchitecture* parent) {
    this->parent = parent;
}

QString MachOArchitecture::getResolvedName(const QString& name, const QString& workingPath) const {
    QString absoluteFileName = MachO::dynamicLoader->getPathname(name, this, workingPath);
    if (!absoluteFileName.isNull())
        return absoluteFileName;
    // return unresolved name if it cannot be resolved to a valid absolute name
    return name;
}

std::vector<QString*> MachOArchitecture::getRPaths() const {
    // try to get it from the parent (recursively)
    std::vector<QString*> prevRPaths = parent?(std::vector<QString*>(parent->getRPaths())):(std::vector<QString*>());
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
            QString resolvedRPath = MachO::dynamicLoader->replacePlaceholder(rPathCommand->getPath(), this);
            if (resolvedRPath.isNull()) {
                resolvedRPath = rPathCommand->getPath();
            }
            rPaths.push_back(new QString(resolvedRPath));
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

    for (std::vector<QString*>::iterator it2 = rPaths.begin();
    it2 != rPaths.end();
    ++it2)
    {
        delete *it2;
    }
}

unsigned int MachOArchitecture::getSize() const {
    return size;
}



