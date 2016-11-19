#ifndef MACHOEXCEPTION_H
#define MACHOEXCEPTION_H

#include "macho_global.h"

class EXPORT MachOException
{
public:
  MachOException(const std::string&);
  const std::string& getCause() { return cause; }
private:
  const std::string cause;
};

#endif // MACHOEXCEPTION_H
