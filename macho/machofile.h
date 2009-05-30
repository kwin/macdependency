#ifndef MACHOFILE_H
#define MACHOFILE_H

#include "MachO_global.h"
#include "internalfile.h"


class MachOFile
{
public:
    MachOFile(const QString&, bool reversedByteOrder = false);
    MachOFile(const MachOFile& file, bool reversedByteOrder);
    ~MachOFile();

    uint32_t readUint32();
    uint32_t readUint32LE();
    uint32_t readUint32BE();

    void readBytes(char* result, size_t size);

    uint32_t getUint32(unsigned int data) {return (reversedByteOrder?reverseByteOrder(data):data);}
    static uint32_t getUint32LE(uint32_t data);
    static uint32_t getUint32BE(uint32_t data);
    QString getFileName() { return file->fileName(); }
    long long int getSize() { return file->size(); }
    void seek(long long int offset) { position = offset; }
    long long int getPosition() {  return position; }

 private:
    static unsigned int convertByteOrder(char* data, bool isBigEndian, unsigned int numberOfBytes);
    static unsigned int reverseByteOrder(unsigned int data);

    InternalFile* file;
    long long int position;
    const bool reversedByteOrder;

};

#endif // MACHOFILE_H
