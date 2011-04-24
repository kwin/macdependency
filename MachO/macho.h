#ifndef MACHO_H
#define MACHO_H

#include "macho_global.h"
#include <list>
#include <CoreFoundation/CoreFoundation.h>

class MachOFile;
class MachOArchitecture;
class Demangler;
class DynamicLoader;

class EXPORT MachO {
private:
	typedef std::list<MachOArchitecture*> MachOArchitectures;
public:
    MachO(const string& fileName, const MachO* parent = 0);
    ~MachO();

    string getFileName() const;
	
	typedef MachOArchitectures::iterator MachOArchitecturesIterator;
	typedef MachOArchitectures::const_iterator MachOArchitecturesConstIterator;
    MachOArchitecturesIterator getArchitecturesBegin();
    MachOArchitecturesIterator getArchitecturesEnd();
    MachOArchitecture* getCompatibleArchitecture(MachOArchitecture* destArchitecture) const;
	MachOArchitecture* getHostCompatibleArchitecture() const;
    unsigned long long getSize() const;
    time_t getLastModificationTime() const;
    string getVersion() const;
    string getName() const;
    //sQIcon getIcon() const;*/
	const MachO* getParent() { return parent;}
    string getPath() const;
    static Demangler* demangler;
    static DynamicLoader* dynamicLoader;
    static int referenceCounter;
private:
	const MachO* parent;
    MachOFile* file;
	MachOArchitectures architectures;
    CFBundleRef bundle;

    string getApplicationInBundle(const string& bundlePath);
    static string extractStringFromCFStringRef(CFStringRef cfStringRef);
    void init(const string& fileName, const MachO* parent);
	
};

#endif // MACHO_H
