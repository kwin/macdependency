#ifndef INTERNALFILE_H
#define INTERNALFILE_H

#include "macho_global.h"

class InternalFile
{

public:
  virtual ~InternalFile();

  static InternalFile* create(InternalFile* file);
  static InternalFile* create(const std::string& filename);
  void release();

  std::string getPath() const;
  std::string getName() const;
  std::string getTitle() const;
  unsigned long long getSize() const;
  bool seek(long long int position);
  std::streamsize read(char* buffer, std::streamsize size);
  long long int getPosition();
  time_t getLastModificationTime() const;

private:
  unsigned int counter;

  InternalFile(const std::string& filename);
  std::ifstream file;
  std::string filename;
  size_t _fileSize;
  time_t _lastWriteTime;
};

#endif // INTERNALFILE_H
