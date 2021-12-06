#include "about.h"
#include "cmd.h"
#include "version.h"
#include <unistd.h>

#include <QApplication>
#include <QFileInfo>
#include <QMessageBox>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>

// display doc as nomal user when run as root
void displayDoc(QString url, QString title)
{
    if (system("command -v mx-viewer >/dev/null") == 0) {
        system("/usr/bin/mx-viewer " + url.toUtf8() + " \"" + title.toUtf8() + "\"&");
    } else {
        if (getuid() != 0) {
            system("/usr/bin/xdg-open " + url.toUtf8());
        } else {
            Cmd cmd;
            QString user = cmd.getCmdOut("/usr/bin/logname", true);
            system("runuser -l " + user.toUtf8() + " -c \"env XDG_RUNTIME_DIR=/run/user/$(id -u " +
                   user.toUtf8() + ") /usr/bin/xdg-open " + url.toUtf8() + "\"&");
        }
    }
}

void displayAboutMsgBox(QString title, QString message, QString licence_url, QString license_title)
{
    QMessageBox msgBox(QMessageBox::NoIcon, title, message);
    QPushButton *btnLicense = msgBox.addButton(QApplication::tr("License"), QMessageBox::HelpRole);
    QPushButton *btnChangelog = msgBox.addButton(QApplication::tr("Changelog"), QMessageBox::HelpRole);
    QPushButton *btnCancel = msgBox.addButton(QApplication::tr("Cancel"), QMessageBox::NoRole);
    btnCancel->setIcon(QIcon::fromTheme("window-close"));

    msgBox.exec();

    if (msgBox.clickedButton() == btnLicense) {
        displayDoc(licence_url, license_title);
    } else if (msgBox.clickedButton() == btnChangelog) {
        QDialog *changelog = new QDialog();
        changelog->setWindowTitle(QApplication::tr("Changelog"));
        changelog->resize(600, 500);

        QTextEdit *text = new QTextEdit;
        text->setReadOnly(true);
        Cmd cmd;
        text->setText(cmd.getCmdOut("zless /usr/share/doc/" + QFileInfo(QCoreApplication::applicationFilePath()).fileName()  + "/changelog.gz"));

        QPushButton *btnClose = new QPushButton(QApplication::tr("&Close"));
        btnClose->setIcon(QIcon::fromTheme("window-close"));
        QApplication::connect(btnClose, &QPushButton::clicked, changelog, &QDialog::close);

        QVBoxLayout *layout = new QVBoxLayout;
        layout->addWidget(text);
        layout->addWidget(btnClose);
        changelog->setLayout(layout);
        changelog->exec();
        delete changelog;
    }
}