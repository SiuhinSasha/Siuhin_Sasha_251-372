QT += core network
TEMPLATE = app
CONFIG += c++17 cmdline
TARGET = votingserver

SOURCES += \
    main.cpp \
    votingserver.cpp

HEADERS += \
    votingserver.h
