#include "machofile.h"
#include "machoexception.h"
#include "internalfile.h"

MachOFile::MachOFile(const string& filename,const MachOFile* parent,  bool reversedByteOrder) :
        file(InternalFile::create(filename)), position(0), reversedByteOrder(reversedByteOrder), parent(parent)
{
    if (parent) {
        executablePath = parent->executablePath;
    } else {
        executablePath = getPath();
    }
}

MachOFile::MachOFile(const MachOFile& file, bool reversedByteOrder) :
        file(InternalFile::create(file.file)), position(file.position), reversedByteOrder(reversedByteOrder), parent(file.parent), executablePath(file.executablePath)
{
}

MachOFile::~MachOFile() {
    file->release();
}

string MachOFile::getPath() const { 
	return file->getPath(); 
}

string MachOFile::getName() const { 
	string filename = file->getName(); 
	return filename;
}
string MachOFile::getTitle() const { return file->getTitle(); }
unsigned long long MachOFile::getSize() const { return file->getSize(); }
time_t MachOFile::getLastModificationTime() const { return file->getLastModificationTime(); }

uint32_t MachOFile::readUint32() {
    unsigned int temp;
    readBytes((char*)&temp, sizeof(temp));
    return getUint32(temp);
}

uint32_t MachOFile::readUint32LE() {
    unsigned int temp;
    readBytes((char*)&temp, sizeof(temp));
    return getUint32LE(temp);
}

uint32_t MachOFile::readUint32BE() {
    unsigned int temp;
    readBytes((char*)&temp, sizeof(temp));
    return getUint32BE(temp);
}

uint32_t MachOFile::getUint32BE(uint32_t data) {
    return convertByteOrder((char*)&data, true, sizeof(data));
}

uint32_t MachOFile::getUint32LE(uint32_t data) {
    return convertByteOrder((char*)&data, false, sizeof(data));
}

void MachOFile::readBytes(char* result, size_t size) {
    if (file->getPosition() != position) {
        file->seek(position);
    }
    if (file->read(result, size) != size)
        throw MachOException("File '" + file->getName() + "' not big enough. Probably no valid Mach-O!");
    position += size;
}

// convert from big endian or little endian to native format (Intel=little endian) and return as unsigned int (32bit)
unsigned int MachOFile::convertByteOrder(char* data, bool isBigEndian, unsigned int numberOfBytes) {

    assert(numberOfBytes> 0);
    assert(numberOfBytes <= 4); // max 4 byte

    unsigned int result = 0;

    // big endian extract (most significant byte first) (will work on little and big-endian computers)
    unsigned int numberOfShifts = isBigEndian ? numberOfBytes - 1 : 0;

    for (unsigned int n = 0; n < numberOfBytes; n++) {
        result |= static_cast<unsigned char>(data[n]) << (8 * numberOfShifts); // the bit shift will do the correct byte order for you
        numberOfShifts += isBigEndian ? -1 : +1;
    }
    return result;
}

uint32_t MachOFile::reverseByteOrder(uint32_t data) {
    char* sourceData = (char*)&data;
    uint32_t result;
    char* destData = (char*)&result;

    destData[3] = sourceData[0];
    destData[2] = sourceData[1];
    destData[1] = sourceData[2];
    destData[0] = sourceData[3];

    return result;
}


