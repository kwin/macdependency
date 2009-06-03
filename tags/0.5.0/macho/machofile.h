#ifndef MACHOFILE_H
#define MACHOFILE_H

#include "MachO_global.h"
#include "internalfile.h"
#include <QtCore/QFileInfo>

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

    uint32_t getUint32(unsigned int data) const {return (reversedByteOrder?reverseByteOrder(data):data);}
    static uint32_t getUint32LE(uint32_t data);
    static uint32_t getUint32BE(uint32_t data);
    QString getPath() const { return QFileInfo(getName()).absolutePath(); }
    QString getName() const { return file->fileName(); }
    long long int getSize() const { return file->size(); }
    void seek(long long int offset) { position = offset; }
    long long int getPosition() const {  return position; }
    const QString& getExecutablePath() const { return *executablePath; }

 private:
    static unsigned int convertByteOrder(char* data, bool isBigEndian, unsigned int numberOfBytes);
    static unsigned int reverseByteOrder(unsigned int data);

    InternalFile* file;
    const QString* executablePath;
    long long int position;
    const bool reversedByteOrder;

};

#endif // MACHOFILE_H
