#ifndef INTERNALFILE_H
#define INTERNALFILE_H

#include "macho_global.h"
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

class InternalFile
{

public:
    static InternalFile* create(InternalFile* file);
    static InternalFile* create(const string& filename);
    void release();

    string getPath() const;
    string getName() const;
	string getTitle() const;
	unsigned long long getSize() const;
    bool seek(long long int position);
    streamsize read(char* buffer, streamsize size);
    long long int getPosition();
	time_t getLastModificationTime() const;

private:
    unsigned int counter;
    virtual ~InternalFile();
    InternalFile(const string& filename);
	boost::filesystem::ifstream file;
	boost::filesystem::path filename;
};

#endif // INTERNALFILE_H
