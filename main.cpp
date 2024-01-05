/**********************************************************************
 *  main.cpp
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

#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QIcon>
#include <QLibraryInfo>
#include <QLocale>
#include <QTranslator>

#include "common.h"
#include "mainwindow.h"
#include <unistd.h>

int main(int argc, char *argv[])
{
    if (getuid() == 0) {
        qputenv("XDG_RUNTIME_DIR", "/run/user/0");
        qunsetenv("SESSION_MANAGER");
    }
    QApplication app(argc, argv);
    if (getuid() == 0) {
        qputenv("HOME", "/root");
    }
    QApplication::setOrganizationName("MX-Linux");
    QApplication::setWindowIcon(QIcon::fromTheme(QApplication::applicationName()));

    QTranslator qtTran;
    if (qtTran.load("qt_" + QLocale::system().name(), QLibraryInfo::location(QLibraryInfo::TranslationsPath))) {
        QApplication::installTranslator(&qtTran);
    }

    QTranslator qtBaseTran;
    if (qtBaseTran.load("qtbase_" + QLocale::system().name(), QLibraryInfo::location(QLibraryInfo::TranslationsPath))) {
        QApplication::installTranslator(&qtBaseTran);
    }

    QTranslator appTran;
    if (appTran.load(QApplication::applicationName() + "_" + QLocale::system().name(),
                     "/usr/share/" + QApplication::applicationName() + "/locale")) {
        QApplication::installTranslator(&appTran);
    }

    // Root guard
    QFile loginUidFile {"/proc/self/loginuid"};
    if (loginUidFile.open(QIODevice::ReadOnly)) {
        QString loginUid = QString(loginUidFile.readAll()).trimmed();
        loginUidFile.close();
        if (loginUid == "0") {
            QMessageBox::critical(
                nullptr, QObject::tr("Error"),
                QObject::tr(
                    "You seem to be logged in as root, please log out and log in as normal user to use this program."));
            exit(EXIT_FAILURE);
        }
    }
    qDebug().noquote() << QApplication::applicationName() << QObject::tr("version:")
                       << QApplication::applicationVersion();
    MainWindow w;
    w.show();
    return QApplication::exec();
}
