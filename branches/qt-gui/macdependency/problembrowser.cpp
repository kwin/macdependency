#include "problembrowser.h"


const QString ProblemBrowser::STATE_PREFIX[ProblemBrowser::NumStates] = { tr(""), tr("Warning: "), tr("Error: ")};
ProblemBrowser::ProblemBrowser(QWidget * parent) :
        QTextBrowser(parent)
{
}

void ProblemBrowser::printError(const QString& text) {
    print(StateError, text);
}

void ProblemBrowser::printWarning(const QString& text) {
    print(StateWarning, text);
}

void ProblemBrowser::print(State state, const QString& text) {
    append(STATE_PREFIX[state] + text);
}
