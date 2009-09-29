#include "dynamicloader.h"
#include "machofile.h"
#include "machoarchitecture.h"

#include <boost/filesystem.hpp>

/*
  This class emulates the search path mechanism of dyld
  http://developer.apple.com/documentation/Darwin/Reference/Manpages/man1/dyld.1.html
  Unfortunately the several documents from apple contradict each other. Therefore I analyzed the source of the original dyld
  http://www.opensource.apple.com/source/dyld/dyld-97.1/src/dyld.cpp
*/

const char* DynamicLoader::EnvironmentPathVariable::PATHS_SEPARATOR = ";";
const char* DynamicLoader::EnvironmentPathVariable::HOME_PATH = getenv("HOME");

DynamicLoader::EnvironmentPathVariable::EnvironmentPathVariable() {
    // default constructor (should not be used explicitly)
}

DynamicLoader::EnvironmentPathVariable::EnvironmentPathVariable(const string& name, const StringList& defaultValues)
{
	const char* envValue = getenv(name.c_str());
	string values;
	if (envValue) {
		values = envValue;
	}

	if (!values.empty()) {
		size_t start = 0;
		size_t end;
		while ((end = values.find(PATHS_SEPARATOR)) != string::npos) {
			addPath(values.substr(start, end-start));
			start = end+1;
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

void DynamicLoader::EnvironmentPathVariable::addPath(const string& path) {
	paths.push_back(path);
	replaceHomeDirectory(paths.back());
}

bool DynamicLoader::EnvironmentPathVariable::replaceHomeDirectory(string& path) {
	size_t homePos = path.find("~");
			if (homePos != string::npos) {
				path.replace(homePos, 1, HOME_PATH);
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
    // init/read out some variables
    for (unsigned int i=0; i < NumEnvironmentVariables; i++) {
        environmentVariables[i] = EnvironmentPathVariable(ENVIRONMENT_VARIABLE_NAMES[i], ENVIRONMENT_VARIABLE_DEFAULT_VALUES[i]);
    } 
}

string DynamicLoader::replacePlaceholder(const string& name, const MachOArchitecture* architecture) const {
    string resolvedName = name;
    if (name.find(PLACEHOLDERS[ExecutablePath]) == 0) {
		resolvedName.replace(0, strlen(PLACEHOLDERS[ExecutablePath]), architecture->getFile()->getExecutablePath());
    } else if (name.find(PLACEHOLDERS[LoaderPath]) == 0) {
        resolvedName.replace(0, strlen(PLACEHOLDERS[LoaderPath]), architecture->getFile()->getPath());
    } else {
        return string();
    }
    return resolvedName;
}

string DynamicLoader::getPathname(const string& name, const MachOArchitecture* architecture, const string& workingPath) const {
    // simple name (only the last part of the name, after the last PATH_SEPARATOR)
    size_t lastSlashPosition = name.rfind(PATH_SEPARATOR);
    string simpleName;
    if (lastSlashPosition != string::npos && lastSlashPosition < name.length() - 1) {
        simpleName = name.substr(lastSlashPosition+1);
    } else {
    	simpleName = name;
    }

    // try LD_LIBRARY_PATH
    string pathName;
    pathName = getExistingPathname(simpleName, environmentVariables[LdLibraryPath]);
    if (!pathName.empty())
        return pathName;

    string frameworkName = getFrameworkName(name);
    if (!frameworkName.empty()) {
        // strip the already contained suffix
        pathName = getExistingPathname(frameworkName, environmentVariables[DyldFrameworkPath]);
        if (!pathName.empty())
            return pathName;
    }

    pathName = getExistingPathname(simpleName, environmentVariables[DyldLibraryPath]);
    if (!pathName.empty())
        return pathName;

    // resolve placeholder
    string resolvedName = replacePlaceholder(name, architecture);
    if (!resolvedName.empty()) {
        pathName = getExistingPathname(resolvedName);
        if (!pathName.empty())
            return pathName;
    }

    if (name.find(PLACEHOLDERS[RPath]) == 0) {
        // substitute @rpath with all -rpath paths up the load chain
        std::vector<string*> rPaths = architecture->getRPaths();

        for (std::vector<string*>::iterator it = rPaths.begin(); it != rPaths.end(); ++it) {
            resolvedName = name;
            resolvedName.replace(0, strlen(PLACEHOLDERS[RPath]), (**it));
            pathName = getExistingPathname(resolvedName);
            if (!pathName.empty())
                return pathName;
        }

        // after checking against all stored rpaths substitute @rpath with LD_LIBRARY_PATH (if it is set)
        EnvironmentPathVariable ldLibraryPaths = environmentVariables[LdLibraryPath];
        if (!ldLibraryPaths.isEmpty()) {
            for (StringList::const_iterator it = ldLibraryPaths.getPaths().begin(); it != ldLibraryPaths.getPaths().end(); ++it) {
                resolvedName = name;
                resolvedName.replace(0, strlen(PLACEHOLDERS[RPath]), (*it));
                pathName = getExistingPathname(resolvedName);
                if (!pathName.empty())
                    return pathName;
            }
        }
    }

    // check pure path (either absolute or relative to working directory)
    if (name.find(PATH_SEPARATOR) == 0) {
        pathName = getExistingPathname(name);
    } else {
        pathName = getExistingPathname(name, workingPath);
    }
    if (!pathName.empty())
        return pathName;

    // try fallbacks (or its defaults)
    if (!frameworkName.empty()) {
        pathName = getExistingPathname(frameworkName, environmentVariables[DyldFallbackFrameworkPath]);
        if (!pathName.empty())
            return pathName;
    }

    return getExistingPathname(name, environmentVariables[DyldFallbackLibraryPath]);
}

// returns the name is of a framework without any preceeding path information if name specifies a framework, otherwise an invalid string
string DynamicLoader::getFrameworkName(const string& name, const bool strippedSuffix) const {
    // fail fast in case of dylibs
    if (name.find(".framework/") == string::npos) {
        return string();
    }

    /*  first look for the form Foo.framework/Foo
        next look for the form Foo.framework/Versions/A/Foo
        A and Foo are arbitrary strings without a slash */

    // get Foo (part after last slash)
    size_t lastSlashPosition = name.rfind(PATH_SEPARATOR);
    if (lastSlashPosition == string::npos || lastSlashPosition == name.length() -1) {
        return false;
    }

    const string foo = name.substr(lastSlashPosition+1);
    const string frameworkPart = foo+".framework/";
    const string framework = frameworkPart + foo;

    if (endsWith(name, framework)) {
        // strip first part
        return framework;
    }
    int startPosition = name.find(frameworkPart+"Versions/");
	bool hasCorrectEnd = endsWith(name, foo);
	
	// TODO: check between Versions/ and foo there must be no additional slash
	if (startPosition != string::npos) {
		if (hasCorrectEnd) {
			return name.substr(startPosition);
		} else if (strippedSuffix == false) {
			// maybe we have a case, where name contains a suffix in foo (which then of course occurs only in the last foo)
			// does foo already contain a suffix?
			size_t suffixStart = foo.rfind("_");
			if (suffixStart != string::npos) {
				string newName = name;
				newName.erase(lastSlashPosition+1+suffixStart);
				return getFrameworkName(newName, true);
			}
		}
	}
	
    // if we are at this part the given name was no framework
    return string();
}

string DynamicLoader::getExistingPathname(const string& name, const EnvironmentPathVariable& environmentPathVariable) const {
    string result;
    const StringList directories = environmentPathVariable.getPaths();
    for (StringList::const_iterator it = directories.begin(); it != directories.end(); ++it) {
    	result = getExistingPathname(name, *it);
        if (!result.empty())
            return result;
    }
    return result;
}

string DynamicLoader::getExistingPathname(const string& file, const string& directory) const {
    string name = file;
    if (!directory.empty()) {
        if (!endsWith(directory, "/")) {
            name = directory + "/" + file;
        } else {
            name = directory + file;
        }
    }
    return getExistingPathname(name);
}

string DynamicLoader::getExistingPathname(const string& name) const {

	boost::filesystem::path path;
    // first try with suffix
    if (!environmentVariables[DyldImageSuffix].isEmpty()) {
        const string suffix = environmentVariables[DyldImageSuffix].getPaths().front();
        string nameWithSuffix = name;
        // where should we append suffix?
		if (endsWith(name, ".dylib")) {
            nameWithSuffix.insert(name.rfind("."), suffix);
        } else {
            nameWithSuffix += suffix;
        }

        path = nameWithSuffix;
        if (boost::filesystem::exists(path)) {
            return nameWithSuffix;
        }
    }

    // then without suffix
    path = name;
    if (boost::filesystem::exists(path)) {
        return name;
    }
    return string();
}

bool DynamicLoader::endsWith(const string& str, const string& substr) {
	size_t i = str.rfind(substr);
	return (i != string::npos) && (i == (str.length() - substr.length()));
}
