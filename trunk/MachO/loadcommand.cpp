#include "loadcommand.h"
#include "dylibcommand.h"
#include "genericcommand.h"
#include "symboltablecommand.h"
#include "rpathcommand.h"
#include "uuidcommand.h"
#include "dylinkercommand.h"
#include "machoexception.h"
#include "machoheader.h"
#include "/usr/include/mach-o/loader.h"

LoadCommand* LoadCommand::getLoadCommand(unsigned int cmd, MachOHeader* header) {

    LoadCommand* loadCommand;
    switch(cmd) {
		case LC_LOAD_DYLINKER:
			loadCommand = new DylinkerCommand(header);
			break;
		case LC_UUID:
			loadCommand = new UuidCommand(header);
			break;
        case LC_LAZY_LOAD_DYLIB:    // dependency is loaded when it is needed
            loadCommand = new DylibCommand(header, DylibCommand::DependencyDelayed);
            break;
        case LC_LOAD_WEAK_DYLIB:    // dependency is allowed to be missing
            loadCommand = new DylibCommand(header, DylibCommand::DependencyWeak);
            break;
        case LC_REEXPORT_DYLIB:
        case LC_LOAD_DYLIB:
            loadCommand = new DylibCommand(header, DylibCommand::DependencyNormal);
            break;
        case LC_ID_DYLIB:
            loadCommand = new DylibCommand(header, DylibCommand::DependencyId);
            break;
        case LC_SYMTAB:
            loadCommand = new SymbolTableCommand(header);
            break;
        case LC_RPATH:
            loadCommand = new RpathCommand(header);
            break;
        default:
            loadCommand = new GenericCommand(header);
            break;
    }
    return loadCommand;
}


LoadCommand::LoadCommand(MachOHeader* header) :
        header(header), file(header->getFile()), offset(file.getPosition()), lcData(0)
{

}

LoadCommand::~LoadCommand() {
    delete[] lcData;
}

void LoadCommand::readLcData() const {
    file.seek(offset + getStructureSize());
    unsigned int size = getSize() - getStructureSize();
    lcData = new char[size];
    file.readBytes(lcData, size);
}

// the offset is not yet in correct byte order here
const char* LoadCommand::getLcDataString(unsigned int offset) const {
    if (!lcData)
        readLcData();

    return lcData+file.getUint32(offset)-getStructureSize();
}
