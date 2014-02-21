TEMPLATE = app
CONFIG += console
CONFIG -= qt

#QMAKE_CXXFLAGS += -std=gnu++0x

SOURCES += main.cpp \
    lodepng.cpp \
    acimage.cpp \
    disttransutil.cpp

HEADERS += \
    lodepng.h \
    acimage.h \
    disttransutil.h

