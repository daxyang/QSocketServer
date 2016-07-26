#-------------------------------------------------
#
# Project created by QtCreator 2016-06-26T05:36:01
#
#-------------------------------------------------

QT       -= gui

TARGET = QSocketServer
TEMPLATE = lib

DEFINES += QSOCKETSERVER_LIBRARY

SOURCES += qsocketserver.cpp \
    sockcmd_treasmit.cpp

HEADERS += qsocketserver.h\
        qsocketserver_global.h \
    net_protocol.h \
    sockcmd_treasmit.h
#arm
unix:!macx {
INCLUDEPATH += ../QSlidingWindow
INCLUDEPATH += ../QSlidingWindowConsume
INCLUDEPATH += ../logfunction
LIBS        += -L/usr/local/arm_lib -lQSlidingWindow -lQSlidingWindowConsume -llog_function
}
#ubuntu
#unix:!macx {
#INCLUDEPATH += ../QSlidingWindow
#INCLUDEPATH += ../QSlidingWindowConsume
#LIBS        += -L/usr/local/linux_lib/lib -lQSlidingWindow -lQSlidingWindowConsume
#}

#install
#arm
unix:!macx {
    target.path = /usr/local/arm_lib
    INSTALLS += target
}

#ubuntu
#unix:!macx {
#target.path = /usr/local/linux_lib/lib
#INSTALLS += target
#}
