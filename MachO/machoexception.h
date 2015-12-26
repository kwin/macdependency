#ifndef MACHOEXCEPTION_H
#define MACHOEXCEPTION_H

#include "MachO_global.h"

class MACHOSHARED_EXPORT MachOException
{
public:
    MachOException(const QString&);
    const QString& getCause() { return cause; }
private:
    const QString cause;
};

#endif // MACHOEXCEPTION_H
