# -------------------------------------------------
# Project created by QtCreator 2009-03-29T23:19:36
# -------------------------------------------------
TARGET = MacDependency
TEMPLATE = app
SOURCES += main.cpp \
    mainwindow.cpp \
    librarytablemodel.cpp \
    dependencyapplication.cpp \
    symboltablemodel.cpp \
    aboutdialog.cpp \
    problembrowser.cpp
HEADERS += mainwindow.h \
    librarytablemodel.h \
    dependencyapplication.h \
    symboltablemodel.h \
    aboutdialog.h \
    problembrowser.h
FORMS += mainwindow.ui \
    aboutdialog.ui
INCLUDEPATH += ../MachO
DEPENDPATH += ../MachO
LIBS += -L../MachO \
    -lmacho
CONFIG += x86 \
    ppc
CONFIG += link_prl
PRE_TARGETDEPS += ../macho/libmacho.a
include(./modeltest/modeltest.pri)
mac:# ICON is still missing
QMAKE_INFO_PLIST = Info_mac.plist
VERSION = 0.5.0
OTHER_FILES += Info_mac.plist
