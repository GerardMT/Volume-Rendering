QT += core opengl

TARGET = Viewer_Gerard
TEMPLATE = app

CONFIG += c++17

LIBS += -lGLEW -lboost_system -lboost_filesystem

SOURCES += \
    histogram_channel_widget.cpp \
    histogram_widget.cpp \
    light.cpp \
    main.cc \
    main_window.cc \
    glwidget.cc \
    camera.cc \
    mesh.cc \
    volume.cc \
    volume_data.cpp

HEADERS  += \
    histogram_channel_widget.h \
    histogram_widget.h \
    histogram_widget_callback.h \
    light.h \
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
    shader/color.frag \
    shader/color.vert \
    shader/histogram.frag \
    shader/histogram.vert \
    shader/histogram_line.frag \
    shader/histogram_line.vert \
    shader/histogram_point.frag \
    shader/histogram_point.vert \
    shader/position.vert \
    shader/position.frag \
    shader/volume.vert \
    shader/volume.frag
