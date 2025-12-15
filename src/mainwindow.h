/**********************************************************************
 *  mainwindow.h
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
#pragma once

#include <QMessageBox>
#include <QProcess>
#include <QSettings>

namespace Ui
{
class MainWindow;
}

class MainWindow : public QDialog
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;
    void centerWindow();
    void setup();

public slots:

private slots:
    void cmdDone();
    void cmdStart();
    void progress(int counter, int duration); // updates progressBar when tick signal is emited
    void pushAboutClicked();
    void pushBackClicked();
    void pushHelpClicked();
    void pushNextClicked();
    void setConnections();
    void setGeneralConnections();
    void updateOutput();

private:
    Ui::MainWindow *ui;
    QProcess proc;
    QSettings settings;
};
