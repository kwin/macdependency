#include "dylibcommand.h"
#include "macho.h"
#include <QtCore/QTime>
#include <QtCore/QStringList>
#include <QtCore/QFileInfo>

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

QString DylibCommand::getName() const {
    return QString(getLcDataString(command.dylib.name.offset));
}

QString DylibCommand::getResolvedName(const QString& workingPath) const {
    QString absoluteFileName = MachO::dynamicLoader->getPathname(getName(), file.getPath(), workingPath);
    if (!absoluteFileName.isNull())
        return absoluteFileName;
    return getName();
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

QString DylibCommand::getVersionString(unsigned int version) {
    return QString("%1.%2.%3").arg(HIWORD(version)).arg((unsigned short)HIBYTE(LOWORD(version))).arg((unsigned short)LOBYTE(LOWORD(version)));
}


