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
    MachO(const QString& fileName, const MachO* parent = 0);
    ~MachO();

    QString getFileName() const { return file->getName(); }
    std::vector<MachOArchitecture*>::iterator getArchitecturesBegin() { return architectures.begin(); }
    std::vector<MachOArchitecture*>::iterator getArchitecturesEnd() { return architectures.end(); }
    MachOArchitecture* MachO::getCompatibleArchitecture(MachOArchitecture* destArchitecture) const;
    long long int getSize() const;
    time_t getLastModificationDate() const;
    QString getBundleVersion() const;
    QString getBundleName() const;
    QIcon getIcon() const;
    QString getPath() const;
    static Demangler* demangler;
    static DynamicLoader* dynamicLoader;
    static int referenceCounter;
private:
    MachOFile* file;
    std::vector<MachOArchitecture*> architectures;
    CFBundleRef bundle;
    QString bundlePath;

    QString getApplicationInBundle(const QString& bundlePath);
    static QString extractStringFromCFStringRef(CFStringRef cfStringRef);
    void init(const QString& fileName, const MachO* parent);
};

#endif // MACHO_H
