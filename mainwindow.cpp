/**********************************************************************
 *  mainwindow.cpp
 **********************************************************************
 * Copyright (C) 2018 MX Authors
 *
 * Authors: Adrian
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

#include <QDebug>
#include <QFileDialog>
#include <QScreen>
#include <QScrollBar>
#include <QTextStream>

#include "about.h"
#include "cmd.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Window); // for the close, min and max buttons
    setGeneralConnections();

    QSize size = this->size();
    if (settings.contains(QStringLiteral("geometry"))) {
        restoreGeometry(settings.value(QStringLiteral("geometry")).toByteArray());
        if (this->isMaximized()) { // add option to resize if maximized
            this->resize(size);
            centerWindow();
        }
    }
    setup();
}

MainWindow::~MainWindow()
{
    settings.setValue(QStringLiteral("geometry"), saveGeometry());
    delete ui;
}

void MainWindow::centerWindow()
{
    QRect screenGeometry = QApplication::primaryScreen()->geometry();
    int x = (screenGeometry.width()-this->width()) / 2;
    int y = (screenGeometry.height()-this->height()) / 2;
    this->move(x, y);
}

// setup versious items first time program runs
void MainWindow::setup()
{
    this->adjustSize();
    ui->pushBack->setHidden(true);;
    ui->stackedWidget->setCurrentIndex(0);
    ui->pushCancel->setEnabled(true);
    ui->pushNext->setEnabled(true);
}

void MainWindow::cmdStart()
{
    setCursor(QCursor(Qt::BusyCursor));
}


void MainWindow::cmdDone()
{
    ui->progressBar->setValue(ui->progressBar->maximum());
    setCursor(QCursor(Qt::ArrowCursor));
}

// set proc and timer connections
void MainWindow::setConnections()
{
    proc.disconnect();
    connect(&proc, &QProcess::readyReadStandardOutput, this, &MainWindow::updateOutput);
    connect(&proc, &QProcess::started, this, &MainWindow::cmdStart);
    connect(&proc, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &MainWindow::cmdDone);
}

void MainWindow::setGeneralConnections()
{
    connect(ui->pushAbout, &QPushButton::clicked, this, &MainWindow::pushAbout_clicked);
    connect(ui->pushBack, &QPushButton::clicked, this, &MainWindow::pushBack_clicked);
    connect(ui->pushHelp, &QPushButton::clicked, this, &MainWindow::pushHelp_clicked);
    connect(ui->pushNext, &QPushButton::clicked, this, &MainWindow::pushNext_clicked);
}

void MainWindow::updateOutput()
{
    const QString out = proc.readAll();
    qDebug() << out;
    ui->outputBox->moveCursor(QTextCursor::End);
    ui->outputBox->insertPlainText(out);
    auto *sb = ui->outputBox->verticalScrollBar();
    sb->setValue(sb->maximum());
    QApplication::processEvents();
}

void MainWindow::progress(int counter, int duration) // processes tick emited by Cmd to be used by a progress bar
{
    ui->progressBar->setMaximum(duration);
    ui->progressBar->setValue(counter % (duration + 1));
}

void MainWindow::pushNext_clicked()
{
    // on first page
    if (ui->stackedWidget->currentIndex() == 0) {
        ui->pushBack->setHidden(false);
        ui->pushBack->setEnabled(true);
        ui->pushNext->setEnabled(false);
        ui->outputLabel->clear();
        ui->stackedWidget->setCurrentWidget(ui->outputPage);

        setConnections();
        Cmd cmd;
        qDebug() << cmd.getCmdOut(QLatin1String(""));
        //qDebug() << getCmdOut(proc, "find / -iname '*user'");
        qDebug() << "DONE";


    // on output page
    } else if (ui->stackedWidget->currentWidget() == ui->outputPage) {

    } else {
        QApplication::quit();
    }
}

void MainWindow::pushBack_clicked()
{
    this->setWindowTitle(QStringLiteral("Custom_Program_Name"));
    ui->stackedWidget->setCurrentIndex(0);
    ui->pushNext->setEnabled(true);
    ui->pushBack->setDisabled(true);
    ui->outputBox->clear();
}


void MainWindow::pushAbout_clicked()
{
    this->hide();
    displayAboutMsgBox( tr("About %1") + tr("Custom_Program_Name"),
                       R"(<p align="center"><b><h2>Custom_Program_Name</h2></b></p><p align="center">)" +
                       tr("Version: ") + QApplication::applicationVersion() + "</p><p align=\"center\"><h3>" +
                       tr("Description goes here") +
                       R"(</h3></p><p align="center"><a href="http://mxlinux.org">http://mxlinux.org</a><br /></p><p align="center">)" +
                       tr("Copyright (c) MX Linux") + "<br /><br /></p>",
                        QStringLiteral("/usr/share/doc/CUSTOMPROGRAMNAME/license.html"), tr("%1 License").arg(this->windowTitle()));

    this->show();
}

// Help button clicked
void MainWindow::pushHelp_clicked()
{
    const QString url = QStringLiteral("google.com");
    displayDoc(url, tr("%1 Help").arg(this->windowTitle()));
}

