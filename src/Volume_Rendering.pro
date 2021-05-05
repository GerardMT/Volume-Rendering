QT += core opengl

greaterThan(QT_MAJOR_VERSION, 5): QT += widgets

TARGET = volume-rendering
TEMPLATE = app

CONFIG += c++17

CONFIG(release, release|debug):DESTDIR = ../build/release/
CONFIG(release, release|debug):OBJECTS_DIR = ../build/release/
CONFIG(release, release|debug):MOC_DIR = ../build/release/
CONFIG(release, release|debug):UI_DIR = ../build/release/

CONFIG(debug, release|debug):DESTDIR = ../build/debug/
CONFIG(debug, release|debug):OBJECTS_DIR = ../build/debug/
CONFIG(debug, release|debug):MOC_DIR = ../build/debug/
CONFIG(debug, release|debug):UI_DIR = ../build/debug/

LIBS += -lGLEW -lboost_system -lboost_filesystem

SOURCES += \
    camera.cpp \
    glwidget.cpp \
    histogram_channel_widget.cpp \
    histogram_widget.cpp \
    light.cpp \
    line.cpp \
    main.cpp \
    main_window.cpp \
    mesh.cpp \
    volume.cpp \
    volume_data.cpp

HEADERS  += \
    histogram_channel_widget.h \
    histogram_widget.h \
    histogram_widget_callback.h \
    light.h \
    line.h \
    main_window.h \
    glwidget.h \
    camera.h \
    paint_gl.h \
    transform.h \
    mesh.h \
    volume.h \
    volume_data.h

FORMS    += \
    main_window.ui

DISTFILES += \
    ../res/shader/color.frag \
    ../res/shader/color.vert \
    ../res/shader/histogram.frag \
    ../res/shader/histogram.vert \
    ../res/shader/histogram_line.frag \
    ../res/shader/histogram_line.vert \
    ../res/shader/histogram_point.frag \
    ../res/shader/histogram_point.vert \
    ../res/shader/line.frag \
    ../res/shader/line.vert \
    ../res/shader/position.vert \
    ../res/shader/position.frag \
    ../res/shader/volume.vert \
    ../res/shader/volume.frag
