#include "machoexception.h"

MachOException::MachOException(const std::string& cause) : cause(cause), warning(false)
{
}

MachOException::MachOException(const std::string& cause, bool warning) : cause(cause), warning(warning)
{
}
