#-------------------------------------------------
#
# Project created by QtCreator 2018-03-20T22:57:43
#
#-------------------------------------------------

QT       -= core gui

TARGET = 3d_render
TEMPLATE = lib
CONFIG += staticlib

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    camera.cpp \
    model.cpp \
    renderer.cpp \
    shader.cpp \
    texture.cpp

HEADERS += \
    camera.h \
    mesh.h \
    model.h \
    renderer.h \
    scene.h \
    shader.h \
    stb_image.h \
    texture.h \
    glm.hpp \
    type_ptr.hpp

LIBS += -lGL -lGLEW -lassimp
unix {
    target.path = /usr/lib
    INSTALLS += target
}