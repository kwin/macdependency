#include "internalfile.h"
#include "machoexception.h"

// use reference counting to reuse files for all used architectures
InternalFile* InternalFile::create(InternalFile* file) {
    file->counter++;
    return file;
}

InternalFile* InternalFile::create(const QString& fileName) {
    return new InternalFile(fileName);
}

void InternalFile::release() {
    counter--;
    if (counter < 1) {
        delete this;
    }
}

InternalFile::InternalFile(const QString& fileName) :
        QFile(fileName), counter(1)
{
    if (!open(QIODevice::ReadOnly))
        throw MachOException("Couldn't open file " + fileName + ": "+ errorString() +" Error Number " + QString::number(error()));
}

// destructor is private since we use reference counting mechanism
InternalFile::~InternalFile()  {
    close();
}
