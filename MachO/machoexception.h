#ifndef MACHOEXCEPTION_H
#define MACHOEXCEPTION_H

#include "macho_global.h"

class EXPORT MachOException
{
public:
  MachOException(const std::string&);
  MachOException(const std::string&, bool warning);
  const std::string& getCause() { return cause; }
  const bool isWarning() { return warning; }
private:
  const std::string cause;
  bool warning;
};

#endif // MACHOEXCEPTION_H
