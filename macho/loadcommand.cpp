#include "loadcommand.h"
#include "dylibcommand.h"
#include "genericcommand.h"
#include "symboltablecommand.h"
#include "machoexception.h"
#include "/usr/include/mach-o/loader.h"

LoadCommand* LoadCommand::getLoadCommand(unsigned int cmd, MachOHeader* header) {

    LoadCommand* loadCommand;
    switch(cmd) {
        case LC_LOAD_DYLIB:
            loadCommand = new DylibCommand(header, true);
            break;
        case LC_ID_DYLIB:
            loadCommand = new DylibCommand(header, false);
            break;
            break;
        case LC_SYMTAB:
            loadCommand = new SymbolTableCommand(header);
            break;
        default:
            loadCommand = new GenericCommand(header);
            break;
    }
    return loadCommand;
}


LoadCommand::LoadCommand(MachOHeader* header) :
        header(header), file(header->getFile()), offset(file.getPosition())
{

}

LoadCommand::~LoadCommand() {


}
