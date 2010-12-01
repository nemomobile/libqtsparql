include(../../sparqltest.pri)

QMAKE_CFLAGS+=-pg
QMAKE_CXXFLAGS+=-pg
QMAKE_LFLAGS+=-pg

CONFIG += qt warn_on console depend_includepath testcase debug
QT += testlib
TARGET = rtcom-benchmark

HEADERS += rtcom-benchmark.h

SOURCES += rtcom-benchmark.cpp
