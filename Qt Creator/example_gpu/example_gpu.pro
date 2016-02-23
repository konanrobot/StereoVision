TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

DESTDIR = bin
TARGET = example_gpu

INCLUDEPATH += /usr/local/include/opencv
LIBS += `pkg-config --cflags --libs opencv` -lopencv_gpu -lopencv_features2d

SOURCES += \
    example_gpu.cpp

OTHER_FILES +=

HEADERS +=

