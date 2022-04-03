QT += core network sql gui
QT += concurrent

TARGET = QtServer
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

CONFIG += debug_and_release console

HEADERS += \
    chatserver.h \
    ltelecomanalyse.h \
    serverworker.h \
    sqliteadapter.h

SOURCES += \
    chatserver.cpp \
    ltelecomanalyse.cpp \
    servermain.cpp \
    serverworker.cpp \
    sqliteadapter.cpp

FORMS +=
