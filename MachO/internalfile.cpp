#include "internalfile.h"
#include "machoexception.h"

// use reference counting to reuse files for all used architectures
InternalFile* InternalFile::create(InternalFile* file) {
    file->counter++;
    return file;
}

InternalFile* InternalFile::create(const std::string& filename) {
	return new InternalFile(filename);
}

void InternalFile::release() {
    counter--;
    if (counter < 1) {
        delete this;
    }
}

InternalFile::InternalFile(const std::string& filename) :
	filename(filename), counter(1)
{
	// open file handle
	file.open(this->filename, std::ios_base::in | std::ios_base::binary);
	if (file.fail()) {
		std::ostringstream error;
		error << "Couldn't open file '" << filename << "'.";
		throw MachOException(error.str());
	}

  struct stat buffer;
  if (stat(filename.c_str(), &buffer) >= 0) {
    _fileSize = buffer.st_size;
    _lastWriteTime = buffer.st_mtime;
  }
}

// destructor is private since we use reference counting mechanism
InternalFile::~InternalFile()  {
    file.close();
}

std::string InternalFile::getPath() const {
	return filename;

}

/* returns whole filename (including path)*/
std::string InternalFile::getName() const {
	// Try to canonicalize path.
  char buffer[PATH_MAX];
	if (realpath(filename.c_str(), buffer) == nullptr)
    return filename;

	return buffer;
}

/* returns filename without path */
std::string InternalFile::getTitle() const {
	return filename;
}

unsigned long long InternalFile::getSize() const {
  return _fileSize;
}

bool InternalFile::seek(long long int position) {
	file.seekg(position, std::ios_base::beg);
	if (file.fail()) {
		file.clear();
		return false;
	}
	return true;
}

std::streamsize InternalFile::read(char* buffer, std::streamsize size) {
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
  return _lastWriteTime;
}
