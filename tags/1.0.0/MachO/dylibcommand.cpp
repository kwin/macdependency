#include "dylibcommand.h"
#include "machofile.h"
#include "machoheader.h"
#include <sstream>

#define HIBYTE(x)	( (unsigned char) ((x) >> 8) )
#define LOBYTE(x)	( (unsigned char) (x) )
#define HIWORD(x)	( (unsigned short) ( (x) >> 16) )
#define LOWORD(x)	( (unsigned short) (x) )

#define MAKEVERSION(x,y,z) 0x00000000 | (x << 16) | (y << 8) | z

DylibCommand::DylibCommand(MachOHeader* header, DependencyType type) :
        LoadCommand(header), type(type)
{
    file.readBytes((char*)&command, sizeof(command));
}

DylibCommand::~DylibCommand() {
}

unsigned int DylibCommand::getSize() const {
    return file.getUint32(command.cmdsize);
}

string DylibCommand::getName() const {
    return string(getLcDataString(command.dylib.name.offset));
}

unsigned int DylibCommand::getCurrentVersion() const {
    return file.getUint32(command.dylib.current_version);
}

unsigned int DylibCommand::getCompatibleVersion() const {
    return file.getUint32(command.dylib.compatibility_version);
}

time_t DylibCommand::getTimeStamp() const {
    return file.getUint32(command.dylib.timestamp);
}

string DylibCommand::getVersionString(unsigned int version) {
    stringstream versionString;
    versionString << HIWORD(version) << "." << (unsigned short)HIBYTE(LOWORD(version)) << "." << (unsigned short)LOBYTE(LOWORD(version));
	return versionString.str();
}


