QT += core network
TEMPLATE = app
CONFIG += c++11 cmdline
TARGET = echoServer

SOURCES += \
    main.cpp \
    echoserver.cpp

HEADERS += \
    echoserver.h
