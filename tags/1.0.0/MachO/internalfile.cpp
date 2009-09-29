#include "internalfile.h"
#include "machoexception.h"
#include <sstream>

using namespace boost::filesystem;

// use reference counting to reuse files for all used architectures
InternalFile* InternalFile::create(InternalFile* file) {
    file->counter++;
    return file;
}

InternalFile* InternalFile::create(const string& filename) {
	return new InternalFile(filename);
}

void InternalFile::release() {
    counter--;
    if (counter < 1) {
        delete this;
    }
}

InternalFile::InternalFile(const string& filename) :
	filename(filename), counter(1)
{
	// open file handle
	file.open(this->filename, ios_base::in|ios_base::binary);
	if (file.fail()) {
		ostringstream error;
		error << "Couldn't open file '" << filename << "'.";
		throw MachOException(error.str());
	}
}

// destructor is private since we use reference counting mechanism
InternalFile::~InternalFile()  {
    file.close();
}

string InternalFile::getPath() const {
	return filename.parent_path().string();

}

/* returns whole filename (including path)*/
string InternalFile::getName() const {
	
	/* unfortunately canonized is not available in newer versions of boost filesystem.
	 For the reasons see the filsystem proposal at http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2005/n1889.html.
	 As an alternative I use realpath but I don't know if it handles unicode strings also.
	 */
	
	// try to canonicalize path
	char* resolvedName = realpath(filename.file_string().c_str(), NULL);
	if (!resolvedName)
		return filename.file_string();
	string resolvedFileName(resolvedName);
	free(resolvedName);
	return resolvedFileName;
}

/* returns filename without path */
string InternalFile::getTitle() const {
	return filename.filename();
}

long long int InternalFile::getSize() const {
	return file_size(filename);
}

bool InternalFile::seek(long long int position) {
	file.seekg(position, ios_base::beg);
	if (file.fail()) {
		file.clear();
		return false;
	}
	return true;
}

streamsize InternalFile::read(char* buffer, streamsize size) {
	file.read(buffer, size);
	if (file.fail()) {
		file.clear();
		return file.gcount();
	}
	// TODO: handle badbit
	return size;
}

long long int InternalFile::getPosition() {
	return file.tellg();
 }

time_t InternalFile::getLastModificationTime() const {
	return last_write_time(filename);
}
