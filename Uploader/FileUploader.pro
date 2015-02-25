#-------------------------------------------------
#
# Project created by QtCreator 2015-02-23T11:26:01
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Monitor
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    monitorworker.cpp \
    workmanager.cpp \
    mainworker.cpp \
    msgqueue.cpp

HEADERS  += mainwindow.h \
    monitorworker.h \
    workmanager.h \
    mainworker.h \
    msgqueue.h \
    config.h

FORMS    += mainwindow.ui

win32:CONFIG(release, debug|release): LIBS += -LE:/ZMQ/lib -llibzmq-v120-mt-4_0_4
else:win32:CONFIG(debug, debug|release): LIBS += -LE:/ZMQ/lib -llibzmq-v120-mt-gd-4_0_4
INCLUDEPATH += E:/ZMQ/include

win32:CONFIG(release, debug|release): LIBS += -LE:/Projects/lib/ -llibcurl
else:win32:CONFIG(debug, debug|release): LIBS += -LE:/Projects/lib/ -llibcurl

INCLUDEPATH += E:/Projects/curl-7.40.0-devel-mingw32/include

