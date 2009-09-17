#ifndef MACHO_GLOBAL_H
#define MACHO_GLOBAL_H

#include <string>

using namespace std;        // macht alles aus dem Namensraum std bekannt

#if defined(MACHO_LIBRARY)
#  define MACHOSHARED_EXPORT 
#else
#  define MACHOSHARED_EXPORT 
#endif

#endif // MACHO_GLOBAL_H
