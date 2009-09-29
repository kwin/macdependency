#ifndef MACHO_GLOBAL_H
#define MACHO_GLOBAL_H

#include <string>

using namespace std;    



// TODO: use visibility options http://developer.apple.com/mac/library/documentation/DeveloperTools/Conceptual/CppRuntimeEnv/Articles/SymbolVisibility.html

#if defined(MACHO_LIBRARY)
#define EXPORT 
#else
#define EXPORT __attribute__((visibility("default")))
#endif

#endif // MACHO_GLOBAL_H
