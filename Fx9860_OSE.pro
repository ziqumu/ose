#-------------------------------------------------
#
# Project created by QtCreator 2013-07-04T14:27:01
#
#-------------------------------------------------

QT       += core gui
QMAKE_CXXFLAGS += -std=c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Fx9860_OSE
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    screen.cpp \
    displaydriver.cpp \
    core.cpp \
    emulator_exception.cpp \
    corethread.cpp \
    refresh.cpp \
    instructions_widget.cpp \
    disassembler.cpp \
    instructionslabel.cpp

HEADERS  += mainwindow.h \
    screen.h \
    displaydriver.h \
    emulator_exception.hpp \
    corethread.h \
    core.h \
    refresh.h \
    instructions_widget.h \
    disassembler.h \
    instructionslabel.h

FORMS    += mainwindow.ui

RESOURCES += \
    ose.qrc
