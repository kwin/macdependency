#ifndef DYLIBCOMMAND_H
#define DYLIBCOMMAND_H

#include "MachO_global.h"

#include "loadcommand.h"
class MACHOSHARED_EXPORT DylibCommand : public LoadCommand
{
public:
    enum DependencyType {
        DependencyWeak,     // dependency is allowed to be missing
        DependencyDelayed,  // dependency is loaded when it is needed (not at start)
        DependencyNormal,
        DependencyId
    };

    DylibCommand(MachOHeader* header, DependencyType type);
    virtual ~DylibCommand();
    virtual unsigned int getSize() const;
    virtual unsigned int getStructureSize() const { return sizeof(command); }
    bool isId() const { return type==DependencyId; }
    bool isNecessary() const { return type!=DependencyWeak; }
    DependencyType getType() const { return type; }
    QString getName() const;
    unsigned int getCurrentVersion() const;
    unsigned int getCompatibleVersion() const;
    time_t getTimeStamp() const;
    static QString getVersionString(unsigned int version);

private:
    dylib_command command;
    DependencyType type;

};

#endif // DYLIBCOMMAND_H
