#ifndef UUIDCOMMAND_H
#define UUIDCOMMAND_H

#include "macho_global.h"
#include "loadcommand.h"
class EXPORT UuidCommand : public LoadCommand
{
public:
    UuidCommand(MachOHeader* header);
    virtual ~UuidCommand();
    virtual unsigned int getSize() const;
    virtual unsigned int getStructureSize() const { return sizeof(command); }
    const uint8_t* getUuid() const;
    
private:
    uuid_command command;

};

#endif // UUIDCOMMAND_H
