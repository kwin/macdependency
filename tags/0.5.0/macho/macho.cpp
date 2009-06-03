#include "macho.h"
#include "machoexception.h"

#include "/usr/include/mach-o/fat.h"

#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtCore/QDateTime>
#include <QtGui/QFileIconProvider>

// static variables
Demangler* MachO::demangler = 0;
DynamicLoader* MachO::dynamicLoader = 0;
int MachO::referenceCounter = 0;

MachO::MachO(const QString& fileName) : bundle(0)
{
    // check if filename is bundle
    QString machOFileName;
    machOFileName = getApplicationInBundle(fileName);
    if (machOFileName.isNull())
        machOFileName = fileName;

    init(machOFileName);

    if (referenceCounter == 0) {
        demangler = new Demangler();
        dynamicLoader = new DynamicLoader();
    }
    referenceCounter++;
}

QString MachO::getApplicationInBundle(const QString& bundlePath) {
    QFileInfo fileInformation(bundlePath);

    QString appPath;
    // check if it is bundle at all
    if (!fileInformation.isBundle() || !fileInformation.isExecutable()) {
        return QString();
    }

    CFURLRef bundleUrl = 0;
    QByteArray utf8BundlePath = bundlePath.toUtf8();

    bundleUrl = CFURLCreateFromFileSystemRepresentation(NULL, (const UInt8 *)utf8BundlePath.data() , utf8BundlePath.length(), true);
    if (bundleUrl != NULL) {
        bundle = CFBundleCreate(NULL, bundleUrl);
        CFRelease(bundleUrl);
        if (bundle != NULL) {
            CFURLRef executableUrl = CFBundleCopyExecutableURL(bundle);
            if (executableUrl != 0) {
                char executableFile[FILENAME_MAX];
                CFURLGetFileSystemRepresentation(executableUrl, true, (UInt8 *)executableFile, FILENAME_MAX);
                appPath = executableFile;
                CFRelease(executableUrl);
                this->bundlePath = bundlePath;
            }
        }
    }
    return appPath;
}

void MachO::init(const QString& fileName)
{
    file = new MachOFile(fileName);
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
            architectures.push_back(architecture);
        }
    } else {
        // seek back to beginning
        file->seek(0);
        MachOArchitecture* architecture = new MachOArchitecture(*file, magic, getSize());
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

    for (std::vector<MachOArchitecture*>::iterator it = architectures.begin();
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
    for (std::vector<MachOArchitecture*>::const_iterator it = architectures.begin();
    it != architectures.end();
    ++it)
    {
        // TODO: match subtypes (only for PowerPC necessary)
        if ((*it)->getHeader()->getCpuType() == destCpuType)
            return  *it;
    }

    return 0;
}

long long int MachO::getSize() const {
    return file->getSize();
}

time_t MachO::getLastModificationDate() const {
    return QFileInfo(file->getName()).lastModified().toTime_t();
}

// return bundle version if available, otherwise NULL string
QString MachO::getBundleVersion() const {
    QString version;
    if (bundle != 0) {
        CFStringRef cfVersion = (CFStringRef)CFBundleGetValueForInfoDictionaryKey(bundle, kCFBundleVersionKey);
        // is version available at all?
        if (cfVersion) {
            version = extractStringFromCFStringRef(cfVersion);
        }
    }
    return version;
}

QString MachO::getBundleName() const {
    QString bundleName;
    if (bundle != 0) {
        CFStringRef cfBundleName = (CFStringRef)CFBundleGetValueForInfoDictionaryKey(bundle, kCFBundleNameKey);
        // is version available at all?
        if (cfBundleName) {
            bundleName = extractStringFromCFStringRef(cfBundleName);
        }
    }
    return bundleName;
}

QString MachO::extractStringFromCFStringRef(CFStringRef cfStringRef) {
    QString string;
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

QIcon MachO::getIcon() const {
    QFileIconProvider iconProvider;
    QFileInfo fileInfo;
    // get icon from bundle
    if (!bundlePath.isNull())
        fileInfo = QFileInfo(bundlePath);
    else
        fileInfo = QFileInfo(getFileName());

    return iconProvider.icon(fileInfo);
}

QString MachO::getPath() const {
    return file->getPath();
}


