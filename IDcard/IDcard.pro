#-------------------------------------------------
#
# Project created by QtCreator 2019-10-18T21:27:28
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = IDcard
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
    mainwindow.cpp \
    preprocess.cpp

HEADERS += \
    mainwindow.h \
    preprocess.h

FORMS += \
        mainwindow.ui

INCLUDEPATH += D:\study\Opencv\opencv3.2.0\opencv\build\include
               D:\study\Opencv\opencv3.2.0\opencv\build\include\opencv
               D:\study\Opencv\opencv3.2.0\opencv\build\include\opencv2
               D:\study\Tesseract\Tesseract-OCR
LIBS += D:\study\Opencv\opencv3.2.0\build-Qt\lib\libopencv_*.a

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    Debug/debug/01.py


#LIBS += -LD:/study/python/libs/ -lpython38

#INCLUDEPATH += D:/study/python/include
#DEPENDPATH += D:/study/python/include

LIBS += -LC:/Users/Siyun/AppData/Local/Programs/Python/Python38-32/libs/ -lpython38

INCLUDEPATH += C:/Users/Siyun/AppData/Local/Programs/Python/Python38-32/include
DEPENDPATH += C:/Users/Siyun/AppData/Local/Programs/Python/Python38-32/include
