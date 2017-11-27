#-------------------------------------------------
#
# Project created by QtCreator 2017-06-05T12:58:31
#
#-------------------------------------------------

QT       += core gui network
CONFIG   += c++14 static
QMAKE_CXXFLAGS += -Wall

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = gui
TEMPLATE = app

INCLUDEPATH += ../robot_control
LIBS += -L../robot_control -lrobot_control


SOURCES += main.cpp\
        mainwindow.cpp \
    myqslider.cpp \
    settingsdialog.cpp \

HEADERS  += mainwindow.h \
    myudpclient.h \
    robotcontroller.h \
    robot.h \
    robotconfiguration.h \
    robotpackets.h \
    robotpositioncontroller.h \
    myqslider.h \
    settingsdialog.h \


FORMS    += mainwindow.ui \
    settingsdialog.ui

