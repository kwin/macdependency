#include "dynamicloader.h"
#include "machofile.h"
#include "machoarchitecture.h"

#include <pwd.h>
#include <stdlib.h>
#include <sstream>

/*
 This class emulates the search path mechanism of dyld
 http://developer.apple.com/documentation/Darwin/Reference/Manpages/man1/dyld.1.html
 Unfortunately the several documents from apple contradict each other. Therefore I analyzed the source of the original dyld
 http://www.opensource.apple.com/source/dyld/dyld-97.1/src/dyld.cpp
 */

const char DynamicLoader::EnvironmentPathVariable::PATHS_SEPARATOR = ':';

DynamicLoader::EnvironmentPathVariable::EnvironmentPathVariable() {
  // never call that explicitly
}

DynamicLoader::EnvironmentPathVariable::EnvironmentPathVariable(const char* homePath, const std::string& name, const StringList& defaultValues)
{
  this->homePath = homePath;
  const char* envValue = getenv(name.c_str());
  std::string values;
  if (envValue) {
    values = envValue;
  }

  if (!values.empty()) {
    std::stringstream v(values);
    std::string item;
    while (std::getline(v, item, PATHS_SEPARATOR)) {
      addPath(item);
    }
  } else {
    setPaths(defaultValues);
  }
}

void DynamicLoader::EnvironmentPathVariable::setPaths(const StringList& paths) {
  this->paths = paths;

  for (StringList::iterator it = this->paths.begin(); it!=this->paths.end(); ++it) {
    replaceHomeDirectory(*it);
  }
}

void DynamicLoader::EnvironmentPathVariable::addPath(const std::string& path) {
  paths.push_back(path);
  replaceHomeDirectory(paths.back());
}

bool DynamicLoader::EnvironmentPathVariable::replaceHomeDirectory(std::string& path) {
  size_t homePos = path.find("~/");
  if (homePos != std::string::npos) {
    path.replace(homePos, 1, homePath);
    return true;
  }
  return false;
}

bool DynamicLoader::EnvironmentPathVariable::isEmpty() const {
  if (!paths.empty()) {
    return paths.front().empty();
  }
  return true;
}

// the order must be the order of the enum!
const char* DynamicLoader::ENVIRONMENT_VARIABLE_NAMES[DynamicLoader::NumEnvironmentVariables] = {
  "LD_LIBRARY_PATH",
  "DYLD_FRAMEWORK_PATH",
  "DYLD_LIBRARY_PATH",
  "DYLD_FALLBACK_FRAMEWORK_PATH",
  "DYLD_FALLBACK_LIBRARY_PATH",
  "DYLD_IMAGE_SUFFIX"
};

const char* DynamicLoader::PLACEHOLDERS[DynamicLoader::NumPlaceholders] = {
  "@executable_path",
  "@loader_path",
  "@rpath"
};

const char* DynamicLoader::PATH_SEPARATOR = "/";

const char* DynamicLoader::DEFAULT_FRAMEWORK_PATH[] = {
  "~/Library/Frameworks",
  "/Library/Frameworks",
  "/Network/Library/Frameworks",
  "/System/Library/Frameworks"
};

const char* DynamicLoader::DEFAULT_LIBRARY_PATH[] = {
  "~/lib",
  "/usr/local/lib",
  "/lib",
  "/usr/lib"
};

// unfortunately cannot make stringList const here, but treat it as const
const StringList DynamicLoader::ENVIRONMENT_VARIABLE_DEFAULT_VALUES[DynamicLoader::NumEnvironmentVariables] = {
  StringList(),
  StringList(),
  StringList(),
  StringList(DEFAULT_FRAMEWORK_PATH, DEFAULT_FRAMEWORK_PATH + sizeof(DEFAULT_FRAMEWORK_PATH) / sizeof(*DEFAULT_FRAMEWORK_PATH)),
  StringList(DEFAULT_LIBRARY_PATH, DEFAULT_LIBRARY_PATH + sizeof(DEFAULT_LIBRARY_PATH) / sizeof(*DEFAULT_LIBRARY_PATH)),
  StringList()
};

DynamicLoader::DynamicLoader()
{
  homePath = strdup(getUserHomeDirectory());
  // init/read out some variables
  for (unsigned int i=0; i < NumEnvironmentVariables; i++) {
    environmentVariables[i] = EnvironmentPathVariable(homePath, ENVIRONMENT_VARIABLE_NAMES[i], ENVIRONMENT_VARIABLE_DEFAULT_VALUES[i]);
  }

}

DynamicLoader::~DynamicLoader() {
  free((void*)homePath);
}

const char* DynamicLoader::getUserHomeDirectory() const {
  struct passwd* pwd = getpwuid(getuid());
  if (pwd)
  {
    return pwd->pw_dir;
  }
  else
  {
    // try the $HOME environment variable
    return getenv("HOME");
  }
}

std::string DynamicLoader::replacePlaceholder(const std::string& name, const MachOArchitecture* architecture) const {
  std::string resolvedName = name;
  if (name.find(PLACEHOLDERS[ExecutablePath]) == 0) {
    resolvedName.replace(0, strlen(PLACEHOLDERS[ExecutablePath]), architecture->getFile()->getExecutablePath());
  } else if (name.find(PLACEHOLDERS[LoaderPath]) == 0) {
    resolvedName.replace(0, strlen(PLACEHOLDERS[LoaderPath]), architecture->getFile()->getPath());
  }
  return resolvedName;
}

std::string DynamicLoader::getPathname(const std::string& name, const MachOArchitecture* architecture, const std::string& workingPath) const {
  // simple name (only the last part of the name, after the last PATH_SEPARATOR)
  size_t lastSlashPosition = name.rfind(PATH_SEPARATOR);
  std::string simpleName;
  if (lastSlashPosition != std::string::npos && lastSlashPosition < name.length() - 1) {
    simpleName = name.substr(lastSlashPosition+1);
  } else {
    simpleName = name;
  }

  // try LD_LIBRARY_PATH
  std::string pathName;
  pathName = getExistingPathname(simpleName, environmentVariables[LdLibraryPath], workingPath);
  if (!pathName.empty())
    return pathName;

  std::string frameworkName = getFrameworkName(name);
  if (!frameworkName.empty()) {
    // strip the already contained suffix
    pathName = getExistingPathname(frameworkName, environmentVariables[DyldFrameworkPath], workingPath);
    if (!pathName.empty())
      return pathName;
  }

  pathName = getExistingPathname(simpleName, environmentVariables[DyldLibraryPath], workingPath);
  if (!pathName.empty())
    return pathName;

  // resolve placeholder
  std::string resolvedName = replacePlaceholder(name, architecture);
  if (!resolvedName.empty()) {
    pathName = getExistingPathname(resolvedName, workingPath);
    if (!pathName.empty())
      return pathName;
  }

  if (name.find(PLACEHOLDERS[Rpath]) == 0) {
    // substitute @rpath with all -rpath paths up the load chain
    std::vector<std::string*> rpaths = architecture->getRpaths();

    for (std::vector<std::string*>::iterator it = rpaths.begin(); it != rpaths.end(); ++it) {
      // rpath may contain @loader_path or @executable_path
      std::string rpath = replacePlaceholder((**it), architecture);
      resolvedName = name;
      resolvedName.replace(0, strlen(PLACEHOLDERS[Rpath]), rpath);
      pathName = getExistingPathname(resolvedName, workingPath);
      if (!pathName.empty())
        return pathName;
    }

    // after checking against all stored rpaths substitute @rpath with LD_LIBRARY_PATH (if it is set)
    EnvironmentPathVariable ldLibraryPaths = environmentVariables[LdLibraryPath];
    if (!ldLibraryPaths.isEmpty()) {
      for (StringList::const_iterator it = ldLibraryPaths.getPaths().begin(); it != ldLibraryPaths.getPaths().end(); ++it) {
        resolvedName = name;
        resolvedName.replace(0, strlen(PLACEHOLDERS[Rpath]), (*it));
        pathName = getExistingPathname(resolvedName, workingPath);
        if (!pathName.empty())
          return pathName;
      }
    }
  }

  // check pure path (either absolute or relative to working directory)
  pathName = getExistingPathname(name, workingPath);
  if (!pathName.empty())
    return pathName;

  // try fallbacks (or its defaults)
  if (!frameworkName.empty()) {
    pathName = getExistingPathname(frameworkName, environmentVariables[DyldFallbackFrameworkPath], workingPath);
    if (!pathName.empty())
      return pathName;
  }

  return getExistingPathname(name, environmentVariables[DyldFallbackLibraryPath], workingPath);
}

// returns the name is of a framework without any preceeding path information if name specifies a framework, otherwise an invalid string
std::string DynamicLoader::getFrameworkName(const std::string& name, const bool strippedSuffix) const {
  // fail fast in case of dylibs
  if (name.find(".framework/") == std::string::npos) {
    return "";
  }

  /*  first look for the form Foo.framework/Foo
   next look for the form Foo.framework/Versions/A/Foo
   A and Foo are arbitrary strings without a slash */

  // get Foo (part after last slash)
  size_t lastSlashPosition = name.rfind(PATH_SEPARATOR);
  if (lastSlashPosition == std::string::npos || lastSlashPosition == name.length() -1) {
    return "";
  }

  const std::string foo = name.substr(lastSlashPosition+1);
  const std::string frameworkPart = foo+".framework/";
  const std::string framework = frameworkPart + foo;

  if (endsWith(name, framework)) {
    // strip first part
    return framework;
  }
  int startPosition = name.find(frameworkPart+"Versions/");
  bool hasCorrectEnd = endsWith(name, foo);

  // TODO: check between Versions/ and foo there must be no additional slash
  if (startPosition != std::string::npos) {
    if (hasCorrectEnd) {
      return name.substr(startPosition);
    } else if (strippedSuffix == false) {
      // maybe we have a case, where name contains a suffix in foo (which then of course occurs only in the last foo)
      // does foo already contain a suffix?
      size_t suffixStart = foo.rfind("_");
      if (suffixStart != std::string::npos) {
        std::string newName = name;
        newName.erase(lastSlashPosition+1+suffixStart);
        return getFrameworkName(newName, true);
      }
    }
  }

  // if we are at this part the given name was no framework
  return "";
}

std::string DynamicLoader::getExistingPathname(const std::string& name, const EnvironmentPathVariable& environmentPathVariable, const std::string& workingPath) const {
  std::string result;
  const StringList directories = environmentPathVariable.getPaths();
  for (StringList::const_iterator it = directories.begin(); it != directories.end(); ++it) {
    result = getExistingPathname(name, *it, workingPath);
    if (!result.empty())
      return result;
  }
  return result;
}

std::string DynamicLoader::getExistingPathname(const std::string& file, const std::string& directory, const std::string& workingPath) const {
  std::string name = file;
  if (!directory.empty()) {
    if (!endsWith(directory, "/")) {
      name = directory + "/" + file;
    } else {
      name = directory + file;
    }
  }
  return getExistingPathname(name, workingPath);
}

std::string DynamicLoader::getExistingPathname(const std::string& name, const std::string& workingPath, bool withSuffix) const {

  // complete path
  std::string usedName = name;
  bool tryAgainWithoutSuffix = false;

  // first try with suffix
  if (withSuffix && !environmentVariables[DyldImageSuffix].isEmpty()) {
    // only one suffix is considered
    const std::string suffix = environmentVariables[DyldImageSuffix].getPaths().front();
    // where should we append suffix?
    if (endsWith(name, ".dylib")) {
      usedName.insert(name.rfind("."), suffix);
    } else {
      usedName += suffix;
    }
    tryAgainWithoutSuffix = true;
  }

  // complete path (with working directory)
  usedName = workingPath + "/" + usedName;

  struct stat buffer;
  if (stat(usedName.c_str(), &buffer) == 0) {
    return usedName;
  } else {
    // try without suffix
    if (tryAgainWithoutSuffix) {
      return getExistingPathname(name, workingPath, false);
    }
  }
  return "";
}

bool DynamicLoader::endsWith(const std::string& str, const std::string& substr) {
  size_t i = str.rfind(substr);
  return (i != std::string::npos) && (i == (str.length() - substr.length()));
}
