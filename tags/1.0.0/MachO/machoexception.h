#ifndef MACHOEXCEPTION_H
#define MACHOEXCEPTION_H

#include "macho_global.h"

class EXPORT MachOException
{
public:
    MachOException(const string&);
    const string& getCause() { return cause; }
private:
    const string cause;
};

#endif // MACHOEXCEPTION_H
