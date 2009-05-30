#ifndef MACHO_H
#define MACHO_H

#include "MachO_global.h"
#include "machofile.h"
#include "machoarchitecture.h"
#include "demangler.h"
#include "dynamicloader.h"

#include <vector>

#include <QtCore/QString>
#include <QtGui/QIcon>
#include <CoreFoundation/CoreFoundation.h>

class MACHOSHARED_EXPORT MachO {
public:
    MachO(const QString& fileName);
    ~MachO();

    QString getFileName() { return file->getFileName(); }
    std::vector<MachOArchitecture*>::iterator getArchitecturesBegin() { return architectures.begin(); }
    std::vector<MachOArchitecture*>::iterator getArchitecturesEnd() { return architectures.end(); }
    MachOArchitecture* MachO::getCompatibleArchitecture(MachOArchitecture* destArchitecture);
    long long int getSize();
    time_t getLastModificationDate();
    QString getVersion();
    QIcon getIcon();
    QString getDirectory();
    static Demangler* demangler;
    static DynamicLoader* dynamicLoader;
    static int referenceCounter;
private:
    MachOFile* file;
    std::vector<MachOArchitecture*> architectures;
    CFBundleRef bundle;
    QString bundlePath;

    QString getApplicationInBundle(const QString& bundlePath);
    void init(const QString& fileName);
};

#endif // MACHO_H
