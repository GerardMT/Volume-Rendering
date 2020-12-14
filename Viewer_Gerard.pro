QT += core opengl

TARGET = Viewer_Gerard
TEMPLATE = app

CONFIG += c++17

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
    shader/color.frag \
    shader/color.vert \
    shader/histogram.frag \
    shader/histogram.vert \
    shader/histogram_line.frag \
    shader/histogram_line.vert \
    shader/histogram_point.frag \
    shader/histogram_point.vert \
    shader/line.frag \
    shader/line.vert \
    shader/position.vert \
    shader/position.frag \
    shader/volume.vert \
    shader/volume.frag
