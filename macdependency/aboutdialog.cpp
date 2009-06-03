#include "aboutdialog.h"
#include "ui_aboutdialog.h"

#include <QtCore/QDate>
#include <QtCore/QSysInfo>
#include <CoreFoundation/CoreFoundation.h>

#define AUTHOR "Konrad Windszus"

AboutDialog::AboutDialog(QWidget *parent) :
        QDialog(parent),
        ui(new Ui::AboutDialog)
{
    ui->setupUi(this);

    QDate date = QDate::fromString(__DATE__, "MMM d yyyy");
    if (date.isNull()) {
        date = QDate::fromString(__DATE__, "MMM  d yyyy");
    }

    const QString description = tr(
            "Version %1\n"
            "Based on Qt %2 (%3 bit)\n"
            "Built on %4 at %5\n"
            "Copyright 2009-%6 %7. All rights reserved.\n"
            "\n"
            "The program is provided AS IS with NO WARRANTY OF ANY KIND,\n"
            "INCLUDING THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A\n"
            "PARTICULAR PURPOSE.")
            .arg(getBundleVersion(), QLatin1String(QT_VERSION_STR), QString::number(QSysInfo::WordSize),
                 date.toString(), QLatin1String(__TIME__), date.toString("yyyy"),
                 (QLatin1String(AUTHOR)));

    ui->descriptionLabel->setText(description);
}

AboutDialog::~AboutDialog()
{
    delete ui;
}

void AboutDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void AboutDialog::on_aboutQtButton_clicked() {
    qApp->aboutQt();
}

QString AboutDialog::getBundleVersion() {
    QString version;
    CFBundleRef bundle = CFBundleGetMainBundle();

    CFStringRef cfVersion = (CFStringRef)CFBundleGetValueForInfoDictionaryKey(bundle, kCFBundleVersionKey);
    // is version available at all?
    if (cfVersion) {
        const char* szVersion = CFStringGetCStringPtr(cfVersion, kCFStringEncodingASCII);
        if (szVersion == NULL) {
            CFIndex versionLength = CFStringGetMaximumSizeForEncoding(CFStringGetLength(cfVersion), kCFStringEncodingASCII);

            char szVersionNew[versionLength + 1];
            if (CFStringGetCString(cfVersion,
                                   szVersionNew,
                                   versionLength+1,
                                   kCFStringEncodingASCII
                                   ))
                version = szVersionNew;
            delete[] szVersion;
        } else {
            version = szVersion;
        }
    }
    return version;
}
