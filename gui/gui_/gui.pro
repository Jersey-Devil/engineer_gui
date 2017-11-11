#-------------------------------------------------
#
# Project created by QtCreator 2017-06-05T12:58:31
#
#-------------------------------------------------

QT       += core gui network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = gui
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    myudpclient.cpp \
    robotcontroller.cpp \
    robot.cpp \
    robotconfiguration.cpp \
    myqslider.cpp \
    settingsdialog.cpp \
    robotpositioncontroller.cpp

HEADERS  += mainwindow.h \
    myudpclient.h\
    robotcontroller.h \
    robot.h \
    robotconfiguration.h \
    robotPackets.h \
    myqslider.h \
    settingsdialog.h \
    robotpositioncontroller.h

FORMS    += mainwindow.ui \
    settingsdialog.ui
