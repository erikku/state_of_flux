# Copyright (C) 2015 John Eric Martin <john.eric.martin@gmail.com>
#
# This file is part of State of Flux.
#
# State of Flux is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# State of Flux is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with State of Flux.  If not, see <http://www.gnu.org/licenses/>.

#-------------------------------------------------
#
# Project created by QtCreator 2015-03-01T12:59:33
#
#-------------------------------------------------

QT       += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = state_of_flux
TEMPLATE = app

RC_FILE += state_of_flux.rc

SOURCES += main.cpp\
        MainWindow.cpp \
    IOSignal.cpp \
    VerilogSignal.cpp \
    IOSignalModel.cpp \
    IOSignalModelInputs.cpp \
    ControlSignal.cpp \
    ControlSignalModel.cpp \
    State.cpp \
    StateModel.cpp \
    VerilogHighlighter.cpp

HEADERS  += MainWindow.h \
    IOSignal.h \
    VerilogSignal.h \
    IOSignalModel.h \
    IOSignalModelInputs.h \
    ControlSignal.h \
    ControlSignalModel.h \
    State.h \
    StateModel.h \
    VerilogHighlighter.h

FORMS    += MainWindow.ui \
    InsertRegister.ui

RESOURCES += \
    state_of_flux.qrc

OTHER_FILES += \
    state_of_flux.rc
