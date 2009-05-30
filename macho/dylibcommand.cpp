#include "dylibcommand.h"
#include "macho.h"
#include <QtCore/QTime>
#include <QtCore/QStringList>
#include <QtCore/QFileInfo>

#define HIBYTE(x)	( (char) ((x) >> 8) )
#define LOBYTE(x)	( (char) (x) )
#define HIWORD(x)	( (short) ( (x) >> 16) )
#define LOWORD(x)	( (short) (x) )

DylibCommand::DylibCommand(MachOHeader* header, bool isDependency) :
      LoadCommand(header), isDependency(isDependency), lcData(0)
{
    file.readBytes((char*)&command, sizeof(command));
}

DylibCommand::~DylibCommand() {
   if (lcData)
       delete[] lcData;
}

unsigned int DylibCommand::getSize() {
    return file.getUint32(command.cmdsize);
}

QString DylibCommand::getName() {
    if (!lcData)
        readLcData();
    char* name;
    name = lcData+file.getUint32(command.dylib.name.offset)-sizeof(command);
    return QString(name);
}

QString DylibCommand::getResolvedName(const QString& workingDirectory) {
    QString absoluteFileName = MachO::dynamicLoader->getPathname(getName(), file.getFileName(), workingDirectory);
    if (!absoluteFileName.isNull())
        return absoluteFileName;
    return getName();
}

unsigned int DylibCommand::getCurrentVersion() {
   return file.getUint32(command.dylib.current_version);
}

unsigned int DylibCommand::getCompatibleVersion() {
    return file.getUint32(command.dylib.compatibility_version);
}

time_t DylibCommand::getTimeStamp() {
    return file.getUint32(command.dylib.timestamp);
}

QString DylibCommand::getVersionString(unsigned int version) {
    return QString("%1.%2.%3").arg(HIWORD(version)).arg((short)HIBYTE(LOWORD(version))).arg((short)LOBYTE(LOWORD(version)));
}

void DylibCommand::readLcData() {
     file.seek(offset + sizeof(command));
     unsigned int size = getSize() - sizeof(command);
     lcData = new char[size];
     file.readBytes(lcData, size);
}
