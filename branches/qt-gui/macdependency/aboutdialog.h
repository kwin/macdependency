#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QtGui/QDialog>

namespace Ui {
    class AboutDialog;
}

class AboutDialog : public QDialog {
    Q_OBJECT
    Q_DISABLE_COPY(AboutDialog)
        public:
            explicit AboutDialog(QWidget *parent = 0);
    virtual ~AboutDialog();

protected:
    virtual void changeEvent(QEvent *e);
private slots:
    void on_aboutQtButton_clicked();

private:
    Ui::AboutDialog *ui;

    QString getBundleVersion();
};

#endif // ABOUTDIALOG_H
