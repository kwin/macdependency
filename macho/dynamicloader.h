#ifndef DYNAMICLOADER_H
#define DYNAMICLOADER_H

#include "MachO_global.h"

#include <QtCore/QStringList>

class DynamicLoader
{
public:
    DynamicLoader();
    virtual ~DynamicLoader() {}

    QString DynamicLoader::getPathname(const QString& name, const QString& callingPathName, const QString& workingDirectory) const;

private:
    class EnvironmentPathVariable
    {
    public:
            EnvironmentPathVariable();
            EnvironmentPathVariable(const char* name, const QStringList& environmentVariables, const QStringList& defaultValues = QStringList());

            bool isEmpty() const;
            const QStringList& getPaths() const { return values; }

    private:
            QStringList splitPathVariable(const QString& pathVariable);
            void setPaths(const QStringList& paths);
            QStringList values;
            static const char* KEY_VALUE_SEPARATOR;
            static const char* PATHS_SEPARATOR;
            static const char* HOME_PATH;
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
        RPath,
        NumPlaceholders
    };

    static const char* PLACEHOLDERS[NumPlaceholders];
    static const char* ENVIRONMENT_VARIABLE_NAMES[NumEnvironmentVariables];
    static const char* PATH_SEPARATOR;
    static QStringList ENVIRONMENT_VARIABLE_DEFAULT_VALUES[NumEnvironmentVariables];

    EnvironmentPathVariable environmentVariables[NumEnvironmentVariables];

    QString getFrameworkName(const QString& name, const bool strippedSuffix = false) const;
    QString getExistingPathname(const QString& name, const EnvironmentPathVariable& environmentPathVariable) const;
    QString getExistingPathname(const QString& name, const QString& directory) const;
    QString getExistingPathname(const QString& name) const;
};

#endif // DYNAMICLOADER_H
