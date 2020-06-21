TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        main.cpp \
        calc.cpp \
        parser.cpp \
        lexer.cpp

HEADERS = calc.h

DISTFILES = lexer.l parser.ypp Makefile.orig

