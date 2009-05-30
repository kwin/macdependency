#ifndef DYLIBCOMMAND_H
#define DYLIBCOMMAND_H

#include "MachO_global.h"

#include "loadcommand.h"
class MACHOSHARED_EXPORT DylibCommand : public LoadCommand
{
public:
    DylibCommand(MachOHeader* header, bool isDependency);
    virtual ~DylibCommand();
    virtual unsigned int getSize();
    bool isId() { return !isDependency; }
    QString getName();
    QString getResolvedName(const QString& workingDirectory);
    unsigned int getCurrentVersion();
    unsigned int getCompatibleVersion();
    time_t getTimeStamp();
    static QString getVersionString(unsigned int version);
private:
    dylib_command command;
    const bool isDependency;
    char* lcData;
    void readLcData();

};

#endif // DYLIBCOMMAND_H
