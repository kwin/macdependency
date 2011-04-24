#include "macho.h"
#include "machoexception.h"
#include "machofile.h"
#include "machoarchitecture.h"
#include "demangler.h"
#include "dynamicloader.h"
#include "machoheader.h"

#include <mach-o/fat.h>

// see http://developer.apple.com/mac/library/documentation/DeveloperTools/Conceptual/MachORuntime/Reference/reference.html 
// for MachO specification

// static variables
Demangler* MachO::demangler = 0;
DynamicLoader* MachO::dynamicLoader = 0;
int MachO::referenceCounter = 0;

MachO::MachO(const string& filename, const MachO* parent) : parent(parent), bundle(NULL)
{
	// check if filename is bundle
    string appFilename = getApplicationInBundle(filename);
    init(appFilename, parent);

    if (referenceCounter == 0) {
        demangler = new Demangler();
        dynamicLoader = new DynamicLoader();
    }
    referenceCounter++;
}

string MachO::getApplicationInBundle(const string& filename) {
    CFURLRef bundleUrl = 0;
	string appFilename = filename;
    bundleUrl = CFURLCreateFromFileSystemRepresentation(NULL, (const UInt8 *)filename.c_str() , filename.length(), true);
    if (bundleUrl != NULL) {
        bundle = CFBundleCreate(NULL, bundleUrl);
        CFRelease(bundleUrl);
        if (bundle != NULL) {
            CFURLRef executableUrl = CFBundleCopyExecutableURL(bundle);
            if (executableUrl != 0) {
                char executableFile[FILENAME_MAX];
                CFURLGetFileSystemRepresentation(executableUrl, true, (UInt8 *)executableFile, FILENAME_MAX);
                appFilename = executableFile;
                CFRelease(executableUrl);
                //this->bundlePath = filename;
            }
        }
    }
    return appFilename;
}

void MachO::init(const string& fileName, const MachO* parent)
{
    MachOFile* parentFile = 0;
    if (parent) {
        parentFile = parent->file;
    }
    file = new MachOFile(fileName, parentFile);
    // read out magic number
    uint32_t magic = file->readUint32();

    // Universal magic is always BE
    if (file->getUint32BE(magic) == FAT_MAGIC) {
        // this is an universal file

        // get number of architecture headers (BE)
        uint32_t numberOfArchitectures = file->readUint32BE();

        // read out all architecture headers
        fat_arch fatArch[numberOfArchitectures];
        file->readBytes((char*)fatArch, sizeof(fat_arch)*numberOfArchitectures);

        // go through all architectures
        for (unsigned int n=0; n < numberOfArchitectures; n++) {
            unsigned int offset = file->getUint32BE(fatArch[n].offset);
            file->seek(offset);
            // read out magic number
            uint32_t magic = file->readUint32();
            file->seek(offset);
            MachOArchitecture* architecture = new MachOArchitecture(*file, magic, file->getUint32BE(fatArch[n].size));
            if (parent)
                architecture->initParentArchitecture(parent->getCompatibleArchitecture(architecture));
            architectures.push_back(architecture);
        }
    } else {
        // seek back to beginning
        file->seek(0);
        MachOArchitecture* architecture = new MachOArchitecture(*file, magic, getSize());
        if (parent)
            architecture->initParentArchitecture(parent->getCompatibleArchitecture(architecture));
        architectures.push_back(architecture);
    }
}


MachO::~MachO() {
    referenceCounter--;
    if (referenceCounter == 0) {
        delete demangler;
        demangler = 0;
        delete dynamicLoader;
        dynamicLoader = 0;
    }

    for (MachOArchitecturesIterator it = architectures.begin();
    it != architectures.end();
    ++it)
    {
        delete *it;
    }
    delete file;
    if (bundle)
        CFRelease(bundle);
}

// choose an architecture which is compatible to the given architecture
MachOArchitecture* MachO::getCompatibleArchitecture(MachOArchitecture* destArchitecture) const {
    MachOHeader::CpuType destCpuType = destArchitecture->getHeader()->getCpuType();

    // go through all architectures
    for (MachOArchitecturesConstIterator it = architectures.begin();
    it != architectures.end();
    ++it)
    {
        // TODO: match subtypes (only for PowerPC necessary)
        if ((*it)->getHeader()->getCpuType() == destCpuType)
            return  *it;
    }
    return 0;
}

MachOArchitecture* MachO::getHostCompatibleArchitecture() const {
	
	unsigned int destCpuType = MachOHeader::getHostCpuType();
	// go through all architectures
    for (MachOArchitecturesConstIterator it = architectures.begin();
		 it != architectures.end();
		 ++it)
    {
        // TODO: match subtypes (only for PowerPC necessary)
        if ((*it)->getHeader()->getCpuType() == destCpuType)
            return  *it;
    }
    return 0;
}

unsigned long long MachO::getSize() const {
    return file->getSize();
}


time_t MachO::getLastModificationTime() const {
    return file->getLastModificationTime();
}

// return bundle version if available, otherwise NULL string
string MachO::getVersion() const {
    string version;
    if (bundle != 0) {
        CFStringRef cfVersion = (CFStringRef)CFBundleGetValueForInfoDictionaryKey(bundle, kCFBundleVersionKey);
        // is version available at all?
        if (cfVersion) {
            version = extractStringFromCFStringRef(cfVersion);
        }
    }
    return version;
}

string MachO::getName() const {
    string name;
    if (bundle != 0) {
        CFStringRef cfBundleName = (CFStringRef)CFBundleGetValueForInfoDictionaryKey(bundle, kCFBundleNameKey);
        // is version available at all?
        if (cfBundleName) {
            name = extractStringFromCFStringRef(cfBundleName);
        } else {
			// take bundle executable name
			CFURLRef bundleUrl = CFBundleCopyExecutableURL(bundle);
			cfBundleName = CFURLCopyLastPathComponent(bundleUrl);
			name = extractStringFromCFStringRef(cfBundleName);
			CFRelease(cfBundleName);
			CFRelease(bundleUrl);
			
		}
    } else {
		name = file->getTitle();
	}
    return name;
}

string MachO::extractStringFromCFStringRef(CFStringRef cfStringRef) {
    string string;
    const char* szString = CFStringGetCStringPtr(cfStringRef, kCFStringEncodingASCII);
    if (szString == NULL) {
        CFIndex stringLength = CFStringGetMaximumSizeForEncoding(CFStringGetLength(cfStringRef), kCFStringEncodingASCII);

        char szStringNew[stringLength + 1];
        if (CFStringGetCString(cfStringRef,
                               szStringNew,
                               stringLength+1,
                               kCFStringEncodingASCII
                               ))
            string = szStringNew;
        delete[] szString;
    } else {
        string = szString;
    }
    return string;
}

string MachO::getPath() const {
    return file->getPath();
}

string MachO::getFileName() const { 
	string filename = file->getName();
	return filename;
}
    
MachO::MachOArchitecturesIterator MachO::getArchitecturesBegin() { return architectures.begin(); }

MachO::MachOArchitecturesIterator MachO::getArchitecturesEnd() { return architectures.end(); }
