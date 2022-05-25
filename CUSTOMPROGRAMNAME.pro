# **********************************************************************
# * Copyright (C) 2018 MX Authors
# *
# * Authors: Adrian
# *          MX Linux <http://mxlinux.org>
# *
# * This is free software: you can redistribute it and/or modify
# * it under the terms of the GNU General Public License as published by
# * the Free Software Foundation, either version 3 of the License, or
# * (at your option) any later version.
# *
# * This program is distributed in the hope that it will be useful,
# * but WITHOUT ANY WARRANTY; without even the implied warranty of
# * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# * GNU General Public License for more details.
# *
# * You should have received a copy of the GNU General Public License
# * along with this package. If not, see <http://www.gnu.org/licenses/>.
# **********************************************************************/

QT       += core gui widgets
CONFIG   += c++17

TARGET = CUSTOMPROGRAMNAME
TEMPLATE = app

# The following define makes your compiler warn you if you use any
# feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += main.cpp\
    mainwindow.cpp \
    about.cpp \
    cmd.cpp

HEADERS  += \
    mainwindow.h \
    version.h \
    about.h \
    cmd.h

FORMS    += \
    mainwindow.ui

TRANSLATIONS += translations/CUSTOMPROGRAMNAME_ca.ts \
                translations/CUSTOMPROGRAMNAME_de.ts \
                translations/CUSTOMPROGRAMNAME_el.ts \
                translations/CUSTOMPROGRAMNAME_es.ts \
                translations/CUSTOMPROGRAMNAME_fr.ts \
                translations/CUSTOMPROGRAMNAME_it.ts \
                translations/CUSTOMPROGRAMNAME_ja.ts \
                translations/CUSTOMPROGRAMNAME_nl.ts \
                translations/CUSTOMPROGRAMNAME_ro.ts \
                translations/CUSTOMPROGRAMNAME_sv.ts

RESOURCES += \
    images.qrc

