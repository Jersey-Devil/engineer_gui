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
INCLUDEPATH += $$PWD/../../glm-0.9.8.5
DEPENDPATH += $$PWD/../robot_control

SOURCES += main.cpp\
        mainwindow.cpp \
    myqslider.cpp \
    settingsdialog.cpp \
    renderwidget.cpp \
    rengine/camera.cpp \
    rengine/model.cpp \
    rengine/renderer.cpp \
    rengine/shader.cpp \
    rengine/texture.cpp

HEADERS  += mainwindow.h \
    myudpclient.h \
    robotcontroller.h \
    robot.h \
    robotconfiguration.h \
    robotpackets.h \
    robotpositioncontroller.h \
    myqslider.h \
    settingsdialog.h \
    renderwidget.h \
    rengine/camera.h \
    rengine/mesh.h \
    rengine/model.h \
    rengine/renderer.h \
    rengine/scene.h \
    rengine/shader.h \
    rengine/stb_image.h \
    rengine/texture.h


FORMS    += mainwindow.ui \
    settingsdialog.ui

LIBS += -lGL -lGLEW -lassimp
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../robot_control/release/ -lrobot_control
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../robot_control/debug/ -lrobot_control
else:unix: LIBS += -L$$OUT_PWD/../robot_control/ -lrobot_control
