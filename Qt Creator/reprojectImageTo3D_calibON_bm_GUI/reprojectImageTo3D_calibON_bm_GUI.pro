#-------------------------------------------------
#
# Project created by QtCreator 2015-10-13T14:57:07
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = reprojectImageTo3D_calibON_bm_GUI
TEMPLATE = app

INCLUDEPATH += /usr/local/include/opencv
LIBS += -L/usr/local/lib -lopencv_core -lopencv_imgcodecs -lopencv_photo -lopencv_superres -lopencv_ts -lopencv_videoio -lopencv_videostab -lopencv_shape -lopencv_stitching -lopencv_imgproc -lopencv_highgui -lopencv_ml -lopencv_video -lopencv_features2d -lopencv_calib3d -lopencv_objdetect -lopencv_flann


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h \
    reprojectImageTo3D.h

FORMS    += mainwindow.ui

OTHER_FILES += \
    config.yml
