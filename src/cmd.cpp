/**********************************************************************
 *  cmd.cpp
 **********************************************************************
 * Copyright (C) 2025 MX Authors
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

#include "cmd.h"

#include <QDebug>
#include <QEventLoop>

Cmd::Cmd(QObject *parent)
    : QProcess(parent)
{
    connect(this, &Cmd::readyReadStandardOutput, [this] { emit outputAvailable(readAllStandardOutput()); });
    connect(this, &Cmd::readyReadStandardError, [this] { emit errorAvailable(readAllStandardError()); });
    connect(this, &Cmd::outputAvailable, [this](const QString &out) { outBuffer += out; });
    connect(this, &Cmd::errorAvailable, [this](const QString &out) { outBuffer += out; });
}

bool Cmd::run(const QString &cmd, bool quiet)
{
    QString output;
    return run(cmd, &output, quiet);
}

QString Cmd::getCmdOut(const QString &cmd, bool quiet)
{
    QString output;
    run(cmd, &output, quiet);
    return output;
}

bool Cmd::run(const QString &cmd, QString *output, bool quiet)
{
    outBuffer.clear();
    if (this->state() != QProcess::NotRunning) {
        qDebug() << "Process already running:" << this->program() << this->arguments();
        return false;
    }
    if (!quiet) {
        qDebug().noquote() << cmd;
    }
    QEventLoop loop;
    connect(this,
            QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this,
            [this](int, QProcess::ExitStatus) { emit done(); },
            Qt::UniqueConnection);
    connect(this, &Cmd::done, &loop, &QEventLoop::quit, Qt::UniqueConnection);
    start(QStringLiteral("/bin/bash"), {QStringLiteral("-c"), cmd});
    if (!waitForStarted()) {
        qDebug() << "Failed to start process:" << cmd;
        return false;
    }
    loop.exec();
    *output = outBuffer.trimmed();
    return (exitStatus() == QProcess::NormalExit && exitCode() == 0);
}
