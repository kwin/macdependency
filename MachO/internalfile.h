#ifndef INTERNALFILE_H
#define INTERNALFILE_H

#include <QtCore/QFile>

class InternalFile : public QFile
{
    Q_OBJECT
public:

    static InternalFile* create(InternalFile* file);
    static InternalFile* create(const QString& fileName);
    void release();
private:
    unsigned int counter;
    virtual ~InternalFile();
    InternalFile(const QString& fileName);
};

#endif // INTERNALFILE_H
