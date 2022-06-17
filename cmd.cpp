#include "cmd.h"

#include <QDebug>
#include <QEventLoop>

Cmd::Cmd(QObject *parent)
    : QProcess(parent)
{
}

bool Cmd::run(const QString &cmd, bool quiet)
{
    QByteArray output;
    return run(cmd, output, quiet);
}

// util function for getting bash command output
QString Cmd::getCmdOut(const QString &cmd, bool quiet)
{
    QByteArray output;
    run(cmd, output, quiet);
    return output;
}

bool Cmd::run(const QString &cmd, QByteArray &output, bool quiet)
{
    if (this->state() != QProcess::NotRunning) {
        qDebug() << "Process already running:" << this->program() << this->arguments();
        return false;
    }
    if (!quiet) qDebug().noquote() << cmd;
    QEventLoop loop;
    connect(this, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), &loop, &QEventLoop::quit);
    start(QStringLiteral("/bin/bash"), {"-c", cmd});
    loop.exec();
    output = readAll().trimmed();
    return (exitStatus() == QProcess::NormalExit && exitCode() == 0);
}

