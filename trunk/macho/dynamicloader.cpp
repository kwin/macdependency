#include "dynamicloader.h"

#include <QtCore/QProcess>
#include <QtCore/QFileInfo>
#include <QtCore/QStringList>

/*
  This class emulates the search path mechanism of dyld
  http://developer.apple.com/documentation/Darwin/Reference/Manpages/man1/dyld.1.html
  Unfortunately the several documents from apple contradict each other. Therefore I analyzed the source of the original dyld
  http://www.opensource.apple.com/source/dyld/dyld-97.1/src/dyld.cpp
*/

const char* DynamicLoader::EnvironmentPathVariable::KEY_VALUE_SEPARATOR = "=";
const char* DynamicLoader::EnvironmentPathVariable::PATHS_SEPARATOR = ";";


DynamicLoader::EnvironmentPathVariable::EnvironmentPathVariable() {
    // default constructor (should not be used explicitly)
}

DynamicLoader::EnvironmentPathVariable::EnvironmentPathVariable(const char* name, const QStringList& environmentVariables, const QStringList& defaultValues)
{
    // environment variables contain several <key>=<value> pairs

    // find name in variables
    int index = environmentVariables.indexOf(QRegExp(QString(name)+"=.*"));
    if (index != -1) {
        // separate value from name
        QString variable = environmentVariables.at(index);

        QStringList parts = variable.split(KEY_VALUE_SEPARATOR);
        if (parts.size() == 2 && !parts.at(1).isEmpty()) {
            // parses and splits at ";"
            QStringList paths = parts.at(1).split(PATHS_SEPARATOR);
            values = paths;
        }
    } else {
        values = defaultValues;
    }
}

bool DynamicLoader::EnvironmentPathVariable::isEmpty() const {
    if (values.size() > 0) {
        return values.at(0).isEmpty();
    }
    return true;
}

QStringList DynamicLoader::EnvironmentPathVariable::splitPathVariable(const QString& pathVariable) {
    return pathVariable.split(PATHS_SEPARATOR, QString::SkipEmptyParts);
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
const char* DynamicLoader::PATH_SEPARATOR = "/";

// unfortunately cannot make QStringList const her, but treat it as const
QStringList DynamicLoader::ENVIRONMENT_VARIABLE_DEFAULT_VALUES[DynamicLoader::NumEnvironmentVariables] = {
    QStringList(),
    QStringList(),
    QStringList(),
    QStringList() << "~/Library/Frameworks" << "/Library/Frameworks" << "/Network/Library/Frameworks" << "/System/Library/Frameworks",
    QStringList() << "~/lib" << "/usr/local/lib" << "/lib" << "/usr/lib",
    QStringList()
};

DynamicLoader::DynamicLoader()
{
    // get environment variables
    QStringList environment = QProcess::systemEnvironment();

    // init/read out some variables
    for (unsigned int i=0; i < NumEnvironmentVariables; i++) {
        environmentVariables[i] = EnvironmentPathVariable(ENVIRONMENT_VARIABLE_NAMES[i], environment, ENVIRONMENT_VARIABLE_DEFAULT_VALUES[i]);
    } 
}

QString DynamicLoader::getPathname(const QString& name, const QString& callingPathName, const QString& workingDirectory) const {
    // simple name (only the last part of the name, after the last PATH_SEPARATOR)
    int lastSlashPosition = name.lastIndexOf(PATH_SEPARATOR);
    QString simpleName = name;
    if (lastSlashPosition != -1 && lastSlashPosition < name.length() - 1) {
        simpleName = simpleName.mid(lastSlashPosition+1);
    }

    // try LD_LIBRARY_PATH
    QString pathName;
    pathName = getExistingPathname(simpleName, environmentVariables[LdLibraryPath]);
    if (!pathName.isEmpty())
        return pathName;

    QString frameworkName = getFrameworkName(name);
    if (!frameworkName.isEmpty()) {
        // strip the already contained suffix
        pathName = getExistingPathname(frameworkName, environmentVariables[DyldFrameworkPath]);
        if (!pathName.isEmpty())
            return pathName;
    }

    pathName = getExistingPathname(simpleName, environmentVariables[DyldLibraryPath]);
    if (!pathName.isEmpty())
        return pathName;

    // resolve executable path
    // TODO: @loader_library & @rpath
    if (name.startsWith("@executable_path")) {
        QString resolvedName = name;
        resolvedName = resolvedName.replace("^@executable_path/", callingPathName);
        pathName = getExistingPathname(resolvedName);
        if (!pathName.isEmpty())
            return pathName;
    }

    // check pure path (either absolute or relative to working directory)
    if (name.startsWith(PATH_SEPARATOR)) {
        pathName = getExistingPathname(name);
    } else {
        pathName = getExistingPathname(name, workingDirectory);
    }
    if (!pathName.isEmpty())
        return pathName;

    // try fallbacks (or its defaults)
    if (!frameworkName.isEmpty()) {
        pathName = getExistingPathname(frameworkName, environmentVariables[DyldFallbackFrameworkPath]);
        if (!pathName.isEmpty())
            return pathName;
    }

    return getExistingPathname(name, environmentVariables[DyldFallbackLibraryPath]);
}

// returns the name is of a framework without any preceeding path information if name specifies a framework, otherwise an invalid QString
QString DynamicLoader::getFrameworkName(const QString& name, const bool strippedSuffix) const {
    // fail fast in case of dylibs
    if (!name.contains(".framework/")) {
        return QString();
    }

    /*  first look for the form Foo.framework/Foo
        next look for the form Foo.framework/Versions/A/Foo
        A and Foo are arbitrary strings without a slash */

    // get Foo (part after last slash)
    int lastSlashPosition = name.lastIndexOf(PATH_SEPARATOR);
    if (lastSlashPosition < 0 || lastSlashPosition == name.length() -1) {
        return false;
    }

    const QString foo = name.mid(lastSlashPosition+1);
    const QString frameworkPart = foo+".framework/";

    if (name.endsWith(frameworkPart+foo)) {
        // strip first part
        return frameworkPart+foo;
    }

    const QString regExp = frameworkPart+"Versions/"+"[^"+PATH_SEPARATOR+"]*"+PATH_SEPARATOR + foo;
    int startPosition = name.indexOf(QRegExp(regExp));

    if (startPosition == -1 && strippedSuffix == false) {
        // maybe we have a case, where name contains a suffix in foo (which then of course occurs only in the last foo)
        // does foo already contain a suffix?
         int suffixStart = foo.lastIndexOf("_");
         if (suffixStart >= 0) {
             QString newName = name;
             newName.remove(lastSlashPosition+1+suffixStart);
             return getFrameworkName(newName, true);
         }
    }

    // strip first part
    return name.mid(startPosition);
}

QString DynamicLoader::getExistingPathname(const QString& name, const EnvironmentPathVariable& environmentPathVariable) const {
    QString result;
    QStringList directories = environmentPathVariable.getPaths();
    for (int i=0; i<directories.size(); i++) {
        result = getExistingPathname(name, directories.at(i));
        if (!result.isNull())
            return result;
    }
    return result;
}

QString DynamicLoader::getExistingPathname(const QString& file, const QString& directory) const {
    QString name = file;
    if (!directory.isEmpty()) {
        if (!directory.endsWith("/")) {
            name = directory + "/" + file;
        } else {
            name = directory + file;
        }
    }
    return getExistingPathname(name);
}

QString DynamicLoader::getExistingPathname(const QString& name) const {
    QFileInfo fileInformation(name);
    // first try with suffix
    if (!environmentVariables[DyldImageSuffix].isEmpty()) {
        const QString suffix = environmentVariables[DyldImageSuffix].getPaths().first();
        QString nameWithSuffix = name;
        // where should we append suffix?
        if (name.endsWith(".dylib")) {
            nameWithSuffix.insert(name.lastIndexOf("."), suffix);
        } else {
            nameWithSuffix = name + suffix;
        }

        fileInformation = nameWithSuffix;
        if (fileInformation.exists()) {
            return nameWithSuffix;
        }
    }

    // then without suffix
    fileInformation = name;
    if (fileInformation.exists()) {
        return name;
    }
    return QString();
}
