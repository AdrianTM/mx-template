/**********************************************************************
 *  mainwindow.cpp
 **********************************************************************
 * Copyright (C) 2017 MX Authors
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



#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QScrollBar>
#include <QTextStream>

#include <QDebug>


MainWindow::MainWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setup();
}

MainWindow::~MainWindow()
{
    delete ui;
}

// setup versious items first time program runs
void MainWindow::setup()
{
    cmd = new Cmd(this);
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


// Get version of the program
QString MainWindow::getVersion(QString name)
{
    Cmd cmd;
    return cmd.getOutput("dpkg -l "+ name + "| awk 'NR==6 {print $3}'");
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
    cmd->disconnect();
    connect(cmd, &Cmd::outputAvailable, this, &MainWindow::updateOutput);
    connect(cmd, &Cmd::started, this, &MainWindow::cmdStart);
    connect(cmd, &Cmd::finished, this, &MainWindow::cmdDone);
}

void MainWindow::updateOutput(const QString &output)
{
    ui->outputBox->insertPlainText(output);
    QScrollBar *sb = ui->outputBox->verticalScrollBar();
    sb->setValue(sb->maximum());
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

        if (cmd->isRunning()) {
            return;
        }


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
    QMessageBox msgBox(QMessageBox::NoIcon,
                       tr("About") + " Custom_Program_Name", "<p align=\"center\"><b><h2>Custom_Program_Name</h2></b></p><p align=\"center\">" +
                       tr("Version: ") + getVersion("CUSTOMPROGRAMNAME") + "</p><p align=\"center\"><h3>" +
                       tr("Description goes here") +
                       "</h3></p><p align=\"center\"><a href=\"http://mxlinux.org\">http://mxlinux.org</a><br /></p><p align=\"center\">" +
                       tr("Copyright (c) MX Linux") + "<br /><br /></p>");
    msgBox.addButton(tr("License"), QMessageBox::AcceptRole);
    msgBox.addButton(tr("Cancel"), QMessageBox::NoRole);
    if (msgBox.exec() == QMessageBox::AcceptRole) {
        QString url = "file:///usr/share/doc/CUSTOMPROGRAMNAME/license.html";
        Cmd c;
        QString user = c.getOutput("logname");
        if (system("command -v mx-viewer") == 0) { // use mx-viewer if available
            system("su " + user.toUtf8() + " -c \"mx-viewer '" + url.toUtf8() + " " + tr("License").toUtf8() + "'\"&");
        } else {
            system("su " + user.toUtf8() + " -c \"xdg-open " + url.toUtf8() + "\"&");
        }
    }
}

// Help button clicked
void MainWindow::on_buttonHelp_clicked()
{
    QString url = "google.com";
    QString exec = "xdg-open";
    if (system("command -v mx-viewer") == 0) { // use mx-viewer if available
        exec = "mx-viewer";
        url += " Custom_Program_Name";
    }
    Cmd c;
    QString user = c.getOutput("logname");
    QString cmd = QString("su " + user + " -c \"" + exec + " " + url + "\"&");
    system(cmd.toUtf8());
}

