#ifndef MACHO_GLOBAL_H
#define MACHO_GLOBAL_H

#include <QtCore/qglobal.h>
#include <QtCore/QString>


#if defined(MACHO_LIBRARY)
#  define MACHOSHARED_EXPORT Q_DECL_EXPORT
#else
#  define MACHOSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // MACHO_GLOBAL_H
