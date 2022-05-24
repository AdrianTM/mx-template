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
        if (getuid() != 0)
            system("/usr/bin/xdg-open " + url.toUtf8());
        else
            system("runuser $(logname) -c \"env XDG_RUNTIME_DIR=/run/user/$(id -u $(logname)) xdg-open " + url.toUtf8() + "\"&");
    }
}

void displayAboutMsgBox(QString title, QString message, QString licence_url, QString license_title)
{
    QMessageBox msgBox(QMessageBox::NoIcon, title, message);
    auto btnLicense = msgBox.addButton(QObject::tr("License"), QMessageBox::HelpRole);
    auto btnChangelog = msgBox.addButton(QObject::tr("Changelog"), QMessageBox::HelpRole);
    auto btnCancel = msgBox.addButton(QObject::tr("Cancel"), QMessageBox::NoRole);
    btnCancel->setIcon(QIcon::fromTheme("window-close"));

    msgBox.exec();

    if (msgBox.clickedButton() == btnLicense) {
        displayDoc(licence_url, license_title);
    } else if (msgBox.clickedButton() == btnChangelog) {
        auto changelog = new QDialog();
        changelog->setWindowTitle(QObject::tr("Changelog"));
        changelog->resize(600, 500);

        auto text = new QTextEdit;
        text->setReadOnly(true);
        Cmd cmd;
        text->setText(cmd.getCmdOut("zless /usr/share/doc/" + QFileInfo(QCoreApplication::applicationFilePath()).fileName()  + "/changelog.gz"));

        auto btnClose = new QPushButton(QObject::tr("&Close"));
        btnClose->setIcon(QIcon::fromTheme("window-close"));
        QObject::connect(btnClose, &QPushButton::clicked, changelog, &QDialog::close);

        auto layout = new QVBoxLayout;
        layout->addWidget(text);
        layout->addWidget(btnClose);
        changelog->setLayout(layout);
        changelog->exec();
        delete changelog;
    }
}
