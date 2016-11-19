#ifndef DYNAMICLOADER_H
#define DYNAMICLOADER_H

#include "macho_global.h"

typedef std::list<std::string> StringList;

class MachOArchitecture;
class DynamicLoader
{
public:
  DynamicLoader();
  virtual ~DynamicLoader();

  std::string replacePlaceholder(const std::string& name, const MachOArchitecture* architecture) const;
  std::string getPathname(const std::string& name, const MachOArchitecture* architecture, const std::string& workingDirectory) const;

private:
  class EnvironmentPathVariable
  {
  public:
    EnvironmentPathVariable();
    EnvironmentPathVariable(const char* homePath, const std::string& name, const StringList& defaultValues = StringList());

    bool isEmpty() const;
    const StringList& getPaths() const { return paths; }

  private:

    void setPaths(const StringList& paths);
    void addPath(const std::string& path);
    bool replaceHomeDirectory(std::string& path);
    StringList paths;
    static const char PATHS_SEPARATOR;
    const char* homePath;
  };

  enum {
    LdLibraryPath,
    DyldFrameworkPath,
    DyldLibraryPath,
    DyldFallbackFrameworkPath,
    DyldFallbackLibraryPath,
    DyldImageSuffix,
    NumEnvironmentVariables
  };

  enum Placeholder {
    ExecutablePath,
    LoaderPath,
    Rpath,
    NumPlaceholders
  };

  static const char* PLACEHOLDERS[NumPlaceholders];
  static const char* ENVIRONMENT_VARIABLE_NAMES[NumEnvironmentVariables];
  static const char* PATH_SEPARATOR;
  static const StringList ENVIRONMENT_VARIABLE_DEFAULT_VALUES[NumEnvironmentVariables];

  static const char* DEFAULT_FRAMEWORK_PATH[];
  static const char* DEFAULT_LIBRARY_PATH[];
  const char* homePath;

  EnvironmentPathVariable environmentVariables[NumEnvironmentVariables];

  std::string getFrameworkName(const std::string& name, const bool strippedSuffix = false) const;
  const char* getUserHomeDirectory() const;

  std::string getExistingPathname(const std::string& name, const EnvironmentPathVariable& environmentPathVariable, const std::string& workingPath) const;
  std::string getExistingPathname(const std::string& name, const std::string& directory, const std::string& workingPath) const;
  std::string getExistingPathname(const std::string& name, const std::string& workingPath, bool withSuffix=true) const;

  static bool endsWith(const std::string& str, const std::string& substr);
};

#endif // DYNAMICLOADER_H
