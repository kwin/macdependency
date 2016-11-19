#include "symboltableentry.h"
#include "macho.h"
#include "machofile.h"

SymbolTableEntry::SymbolTableEntry(MachOFile& file, char* stringTable)
: file(file), stringTable(stringTable)
{
}

SymbolTableEntry::~SymbolTableEntry() {

}

std::string SymbolTableEntry::getName(bool shouldDemangle) const {
  const char *name = getInternalName();
  std::string result = name;
  if (shouldDemangle) {
    int status = 0;

    // Convert real name to readable string. +1 to skip the leading underscore.
    char *realName = abi::__cxa_demangle(name + 1, nullptr, nullptr, &status);
    if (realName != nullptr)
      result = realName;
    free(realName);
  }
  return result;
}

SymbolTableEntry::Type SymbolTableEntry::getType() const {
  unsigned int type = getInternalType();

  if (type & N_STAB) {
    return TypeDebug;
  }
  if (type & N_PEXT) {
    return TypePrivateExtern;
  }
  if (type & N_EXT) {
    if ((type & N_TYPE) == N_UNDF)
      return TypeImported;
    else
      return TypeExported;
  }
  else
    return TypeLocal;
}
