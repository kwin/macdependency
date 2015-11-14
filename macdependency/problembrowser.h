#ifndef PROBLEMBROWSER_H
#define PROBLEMBROWSER_H

#include <QtGui/QTextBrowser>

class ProblemBrowser : public QTextBrowser
{
public:
    ProblemBrowser(QWidget* parent = 0);

    void printError(const QString& text);
    void printWarning(const QString& text);

    // with increasing severity
    enum State {
        StateNormal,
        StateWarning,
        StateError,
        NumStates
    };
    void print(State type, const QString& text);
private:
    static const QString STATE_PREFIX[NumStates];
};

#endif // PROBLEMBROWSER_H
