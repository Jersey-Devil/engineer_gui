#-------------------------------------------------
#
# Project created by QtCreator 2017-06-05T12:58:31
#
#-------------------------------------------------

QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = gui
TEMPLATE = app

CONFIG += c++1z
QMAKE_LFLAGS += -Wl,--no-allow-shlib-undefined,--no-undefined
QMAKE_LFLAGS_RELEASE = "-Wl,-rpath=\'\$$ORIGIN\'"

INCLUDEPATH += $$PWD/../robot_control
DEPENDPATH += $$PWD/../robot_control

SOURCES += main.cpp\
    mainwindow.cpp \
    myqslider.cpp \
    settingsdialog.cpp \
    renderwidget.cpp \
    rendersettings.cpp

HEADERS  += mainwindow.h \
    myudpclient.h \
    robotcontroller.h \
    robot.h \
    robotconfiguration.h \
    robotpackets.h \
    robotpositioncontroller.h \
    myqslider.h \
    settingsdialog.h \
    rendersettings.h \
    renderwidget.h


FORMS    += mainwindow.ui \
    settingsdialog.ui \
    rendersettings.ui

LIBS += -lGL -lGLEW -lassimp

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../robot_control/release/ -lrobot_control
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../robot_control/debug/ -lrobot_control
else:unix: LIBS += -L$$OUT_PWD/../robot_control/ -lrobot_control

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../3d_render/release/ -l3d_render
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../3d_render/debug/ -l3d_render
else:unix: LIBS += -L$$OUT_PWD/../3d_render/ -l3d_render

INCLUDEPATH += $$PWD/../3d_render
DEPENDPATH += $$PWD/../3d_render

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../3d_render/release/lib3d_render.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../3d_render/debug/lib3d_render.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../3d_render/release/3d_render.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../3d_render/debug/3d_render.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../3d_render/lib3d_render.a
