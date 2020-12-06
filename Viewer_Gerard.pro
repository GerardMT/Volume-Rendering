QT += core opengl

TARGET = Viewer_Gerard
TEMPLATE = app

CONFIG += c++17

LIBS += -lGLEW -lboost_system -lboost_filesystem

SOURCES += \
    main.cc \
    main_window.cc \
    glwidget.cc \
    camera.cc \
    mesh.cc \
    volume.cc \
    volume_data.cpp

HEADERS  += \
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
    shader/position.vert \
    shader/position.frag \
    shader/volume.vert \
    shader/volume.frag
