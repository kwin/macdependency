#include "demangler.h"
#include <QtCore/QDebug>

Demangler::Demangler()
{
    connect(this, SIGNAL(error(QProcess::ProcessError)), this, SLOT(error(QProcess::ProcessError)));
    connect(this, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(finished(int, QProcess::ExitStatus)));
    connect(this, SIGNAL(stateChanged(QProcess::ProcessState)), this, SLOT(stateChanged(QProcess::ProcessState)));
}

Demangler::~Demangler()
{
    if (isOpen()) {
        close();
    }
}

void Demangler::error(QProcess::ProcessError error) {
    qDebug() << "some error occurred" << error;
}

void Demangler::finished(int exitCode, QProcess::ExitStatus exitStatus) {
    qDebug() << "process finished with status" << exitStatus;
}

void Demangler::stateChanged(QProcess::ProcessState newState) {
    qDebug() << "state changed to" << newState;
}

QString Demangler::demangleName(const char* name) {
    if (state() != QProcess::Running) {
        close();
        init();
    }
    /*
    qDebug(name); //<< "try to resolve" << name << "in thread" << QThread::currentThread();
    QByteArray buffer;
    buffer.append(name);
    buffer.append("\n");
    writtenBytes += write(buffer);
    */
    write(name);
    if (!waitForBytesWritten()) {
        qDebug() << "Could not wait for the bytes1 to be written:" << errorString() << "exit status" << exitStatus();
    }
    write("\n");
    if (!waitForBytesWritten()) {
        qDebug() << "Could not wait for the bytes2 to be written:" << errorString() << "exit status" << exitStatus();
    }

    if (!waitForReadyRead(100)) {
        qDebug() << "Could not wait for read " << errorString() << " exit status" << exitStatus();
        //qDebug() << "Written bytes (not trying to restart)" << writtenBytes << "read bytes" << readBytes;
        // try to restart process
        return demangleName(name);
    }

    QString test = readAllStandardOutput();
    readBytes += test.length();
    //qDebug() << test;
    //qDebug() << "standard error" << readAllStandardError() << "standard output" << test;

    // remove linebreak
    test.remove(test.length()-1, 1);
    //QString test = demangleProcess.readAllStandardOutput();
    return test;
}

void Demangler::init() {
    writtenBytes = 0;
    readBytes = 0;
    start("c++filt", QStringList() << "--strip-underscore");
    if (!waitForStarted())
        qDebug("Could not start process");
    setReadChannel(QProcess::StandardOutput);
    qDebug() << "PID:" << this->pid();
}

// for gdb this is necessary (only for debugging purposes)
void Demangler::waitForDone() {
    QString out;
    do {
        waitForReadyRead(); // reading symbols...
        out = readAllStandardOutput();
        qDebug() << out;
    } while (!out.contains(" done"));
}
