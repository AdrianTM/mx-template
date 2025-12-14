/**********************************************************************
 *
 **********************************************************************
 * Copyright (C) 2023 MX Authors
 *
 * Authors: Adrian <adrian@mxlinux.org>
 *          MX Linux <http://mxlinux.org>
 *
 * This is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this package. If not, see <http://www.gnu.org/licenses/>.
 **********************************************************************/
#include "about.h"

#include <QApplication>
#include <QDebug>
#include <QFileInfo>
#include <QMessageBox>
#include <QProcess>
#include <QPushButton>
#include <QStandardPaths>
#include <QTextEdit>
#include <QVBoxLayout>

#include "common.h"
#include <unistd.h>

// Display doc as normal user when run as root
void displayDoc(const QString &url, const QString &title)
{
    bool startedAsRoot = false;
    if (getuid() == 0) {
        startedAsRoot = true;
        qputenv("HOME", startingHome.toUtf8()); // Use original home for theming purposes
    }
    // Prefer mx-viewer otherwise use xdg-open (use runuser to run that as logname user)
    const auto executablePath = QStandardPaths::findExecutable(QStringLiteral("mx-viewer"));
    if (!executablePath.isEmpty()) {
        QProcess::startDetached(QStringLiteral("mx-viewer"), {url, title});
    } else {
        if (getuid() != 0) {
            QProcess::startDetached(QStringLiteral("xdg-open"), {url});
        } else {
            QProcess proc;
            proc.start(QStringLiteral("logname"), {}, QIODevice::ReadOnly);
            proc.waitForFinished(5000);
            const auto user = QString::fromUtf8(proc.readAllStandardOutput()).trimmed();
            if (user.isEmpty() || proc.exitStatus() != QProcess::NormalExit) {
                qDebug() << "Failed to get login user, falling back to xdg-open";
                QProcess::startDetached(QStringLiteral("xdg-open"), {url});
            } else {
                QProcess::startDetached(
                    QStringLiteral("runuser"),
                    {QStringLiteral("-u"), user, QStringLiteral("--"), QStringLiteral("xdg-open"), url});
            }
        }
    }
    if (startedAsRoot) {
        qputenv("HOME", "/root");
    }
}

void displayAboutMsgBox(const QString &title, const QString &message, const QString &licenseUrl,
                        const QString &licenseTitle)
{
    QMessageBox msgBox(QMessageBox::NoIcon, title, message);
    auto *btnLicense = msgBox.addButton(QObject::tr("License"), QMessageBox::HelpRole);
    auto *btnChangelog = msgBox.addButton(QObject::tr("Changelog"), QMessageBox::HelpRole);
    auto *btnCancel = msgBox.addButton(QObject::tr("Cancel"), QMessageBox::NoRole);
    btnCancel->setIcon(QIcon::fromTheme("window-close"));

    msgBox.exec();

    if (msgBox.clickedButton() == btnLicense) {
        displayDoc(licenseUrl, licenseTitle);
    } else if (msgBox.clickedButton() == btnChangelog) {
        QDialog changelog;
        changelog.setWindowTitle(QObject::tr("Changelog"));
        changelog.resize(600, 500);

        auto *text = new QTextEdit(&changelog);
        text->setReadOnly(true);
        QProcess proc;
        proc.start(
            "zless",
            {"/usr/share/doc/" + QFileInfo(QCoreApplication::applicationFilePath()).fileName() + "/changelog.gz"},
            QIODevice::ReadOnly);
        proc.waitForFinished(5000);
        text->setText(proc.readAllStandardOutput());

        auto *btnClose = new QPushButton(QObject::tr("&Close"), &changelog);
        btnClose->setIcon(QIcon::fromTheme("window-close"));
        QObject::connect(btnClose, &QPushButton::clicked, &changelog, &QDialog::close);

        auto *layout = new QVBoxLayout(&changelog);
        layout->addWidget(text);
        layout->addWidget(btnClose);
        changelog.setLayout(layout);
        changelog.exec();
    }
}
