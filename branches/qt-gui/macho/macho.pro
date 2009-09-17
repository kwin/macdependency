# -------------------------------------------------
# Project created by QtCreator 2009-03-29T23:39:57
# -------------------------------------------------
# QT -= gui
TARGET = macho
TEMPLATE = lib
CONFIG += staticlib
DEFINES += MACHO_LIBRARY
SOURCES += macho.cpp \
    machoexception.cpp \
    machoheader.cpp \
    machofile.cpp \
    macho32header.cpp \
    macho64header.cpp \
    machoarchitecture.cpp \
    loadcommand.cpp \
    dylibcommand.cpp \
    genericcommand.cpp \
    symboltablecommand.cpp \
    symboltableentry.cpp \
    symboltableentry32.cpp \
    internalfile.cpp \
    symboltableentry64.cpp \
    demangler.cpp \
    dynamicloader.cpp \
    rpathcommand.cpp
HEADERS += macho.h \
    macho_global.h \
    machoexception.h \
    machoheader.h \
    machofile.h \
    macho32header.h \
    macho64header.h \
    machoarchitecture.h \
    loadcommand.h \
    dylibcommand.h \
    genericcommand.h \
    symboltablecommand.h \
    symboltableentry.h \
    symboltableentry32.h \
    internalfile.h \
    symboltableentry64.h \
    demangler.h \
    dynamicloader.h \
    rpathcommand.h
CONFIG += x86 \
    ppc \
    \ \
    \ \
    \ \
    \ \
    \ \
    \ \
    \ \
    \ \ # x86_64 ppc64
    CONFIG \
    += \
    create_prl
VERSION = 0.6.0
