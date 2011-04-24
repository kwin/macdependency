#ifndef MACHOFILE_H
#define MACHOFILE_H

#include "macho_global.h"

class InternalFile;
class MachOFile
{
public:
    MachOFile(const string& filename, const MachOFile* parent, bool reversedByteOrder = false);
    MachOFile(const MachOFile& file, bool reversedByteOrder);
    ~MachOFile();

    uint32_t readUint32();
    uint32_t readUint32LE();
    uint32_t readUint32BE();

    void readBytes(char* result, size_t size);

    uint32_t getUint32(unsigned int data) const {return (reversedByteOrder?reverseByteOrder(data):data);}
    static uint32_t getUint32LE(uint32_t data);
    static uint32_t getUint32BE(uint32_t data);
    string getPath() const;
    string getName() const;
	string getTitle() const;
    unsigned long long getSize() const;
    void seek(long long int offset) { position = offset; }
    long long int getPosition() const {  return position; }
    const string& getExecutablePath() const { return executablePath; }
	time_t getLastModificationTime() const;

 private:
    static unsigned int convertByteOrder(char* data, bool isBigEndian, unsigned int numberOfBytes);
    static unsigned int reverseByteOrder(unsigned int data);

    InternalFile* file;
    long long int position;
    const bool reversedByteOrder;
    const MachOFile* parent;
    string executablePath;

};

#endif // MACHOFILE_H
