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

#include <QFileDialog>
#include <QScrollBar>
#include <QTextStream>

#include <QDebug>

#include <about.h>
#include <cmd.h>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "version.h"

MainWindow::MainWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MainWindow)
{
    qDebug() << "Program Version:" << VERSION;
    ui->setupUi(this);
    setWindowFlags(Qt::Window); // for the close, min and max buttons
    setup();
}

MainWindow::~MainWindow()
{
    delete ui;
}

// setup versious items first time program runs
void MainWindow::setup()
{
    connect(qApp, &QApplication::aboutToQuit, this, &MainWindow::cleanup);
    this->setWindowTitle("Custom_Program_Name");
    this->adjustSize();
    ui->buttonBack->setHidden(true);;
    ui->stackedWidget->setCurrentIndex(0);
    ui->buttonCancel->setEnabled(true);
    ui->buttonNext->setEnabled(true);
}

// cleanup environment when window is closed
void MainWindow::cleanup()
{

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
    connect(&proc, static_cast<void (QProcess::*)(int)>(&QProcess::finished), this, &MainWindow::cmdDone);
}

void MainWindow::updateOutput()
{
    QString out = proc.readAll();
    qDebug() << out;
    ui->outputBox->insertPlainText(out);
    QScrollBar *sb = ui->outputBox->verticalScrollBar();
    sb->setValue(sb->maximum());
    qApp->processEvents();
}

void MainWindow::progress(int counter, int duration) // processes tick emited by Cmd to be used by a progress bar
{
    ui->progressBar->setMaximum(duration);
    ui->progressBar->setValue(counter % (duration + 1));
}

// Next button clicked
void MainWindow::on_buttonNext_clicked()
{
    // on first page
    if (ui->stackedWidget->currentIndex() == 0) {
        ui->buttonBack->setHidden(false);
        ui->buttonBack->setEnabled(true);
        ui->buttonNext->setEnabled(false);
        ui->outputLabel->clear();
        ui->stackedWidget->setCurrentWidget(ui->outputPage);

        setConnections();
        Cmd cmd;
        qDebug() << cmd.getCmdOut("");
        //qDebug() << getCmdOut(proc, "find / -iname '*user'");
        qDebug() << "DONE";


    // on output page
    } else if (ui->stackedWidget->currentWidget() == ui->outputPage) {

    } else {
        return qApp->quit();
    }
}

void MainWindow::on_buttonBack_clicked()
{
    this->setWindowTitle("Custom_Program_Name");
    ui->stackedWidget->setCurrentIndex(0);
    ui->buttonNext->setEnabled(true);
    ui->buttonBack->setDisabled(true);
    ui->outputBox->clear();
}


// About button clicked
void MainWindow::on_buttonAbout_clicked()
{
    this->hide();
    displayAboutMsgBox( tr("About %1") + "Custom_Program_Name",
                       "<p align=\"center\"><b><h2>Custom_Program_Name</h2></b></p><p align=\"center\">" +
                       tr("Version: ") + VERSION + "</p><p align=\"center\"><h3>" +
                       tr("Description goes here") +
                       "</h3></p><p align=\"center\"><a href=\"http://mxlinux.org\">http://mxlinux.org</a><br /></p><p align=\"center\">" +
                       tr("Copyright (c) MX Linux") + "<br /><br /></p>",
                        "/usr/share/doc/CUSTOMPROGRAMNAME/license.html", tr("%1 License").arg(this->windowTitle()), false);

    this->show();
}

// Help button clicked
void MainWindow::on_buttonHelp_clicked()
{
    QString url = "google.com";
    displayDoc(url, tr("%1 Help").arg(this->windowTitle()), false);
}

