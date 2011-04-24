#include "machoarchitecture.h"
#include "machofile.h"
#include "machoheader.h"
#include "loadcommand.h"
#include "dylibcommand.h"
#include "machoexception.h"
#include "rpathcommand.h"
#include "uuidcommand.h"
#include "dylinkercommand.h"
#include "macho.h"
#include "dynamicloader.h"


MachOArchitecture::MachOArchitecture(MachOFile& file, uint32_t magic, unsigned int size) :
    header(MachOHeader::getHeader(file, magic)), file(header->getFile()), size(size), hasReadLoadCommands(false), parent(0), dynamicLibIdCommand(0), uuid(0)
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

std::vector<string*> MachOArchitecture::getRpaths(bool recursively) const {
    // try to get it from the parent (recursively)
	std::vector<string*> prevRpaths;
	if (recursively && parent) {
		prevRpaths = parent->getRpaths(recursively);
	} else {
		prevRpaths = std::vector<string*>();
	}
    // add own rpaths to the end
    prevRpaths.insert(prevRpaths.end(), rpaths.begin(), rpaths.end());
    return prevRpaths;
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
        
		// for dylibCommand...
		DylibCommand* dylibCommand = dynamic_cast<DylibCommand*>(loadCommand);
        if (dylibCommand != 0 && dylibCommand->isId()) {
            dynamicLibIdCommand = dylibCommand;
        }

		// for rpath command...
        RpathCommand* rpathCommand = dynamic_cast<RpathCommand*>(loadCommand);
        if (rpathCommand != 0) {
            // try to replace placeholder
            string resolvedRpath = MachO::dynamicLoader->replacePlaceholder(rpathCommand->getPath(), this);
            if (resolvedRpath.empty()) {
                resolvedRpath = rpathCommand->getPath();
            }
            rpaths.push_back(new string(resolvedRpath));
        }
		
		// for uuid command...
		UuidCommand* uuidCommand = dynamic_cast<UuidCommand*>(loadCommand);
        if (uuidCommand != 0) {
			uuid = uuidCommand->getUuid();
        }
		
		// for dylinker command
		DylinkerCommand* dylinkerCommand = dynamic_cast<DylinkerCommand*>(loadCommand);
        if (dylinkerCommand != 0) {
			dylinker = dylinkerCommand->getName();
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

    for (std::vector<string*>::iterator it2 = rpaths.begin();
    it2 != rpaths.end();
    ++it2)
    {
        delete *it2;
    }
}

unsigned int MachOArchitecture::getSize() const {
    return size;
}

const uint8_t* MachOArchitecture::getUuid() const {
	return uuid;
}



