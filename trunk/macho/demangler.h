#ifndef DEMANGLER_H
#define DEMANGLER_H

#include <QtCore/QProcess>

class Demangler : private QProcess
{
    Q_OBJECT
public:
    Demangler();
    virtual ~Demangler();

    QString demangleName(const char* name);
private:
    int writtenBytes;
    int readBytes;
    void init();

    void waitForDone();
private slots:
    void error(QProcess::ProcessError error);
    void finished(int exitCode, QProcess::ExitStatus exitStatus);
    void stateChanged(QProcess::ProcessState newState);
};

#endif // DEMANGLER_H
