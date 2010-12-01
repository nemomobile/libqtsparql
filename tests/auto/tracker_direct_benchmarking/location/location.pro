include(../../sparqltest.pri)

QMAKE_CFLAGS+=-pg
QMAKE_CXXFLAGS+=-pg
QMAKE_LFLAGS+=-pg

CONFIG += qt warn_on console depend_includepath testcase debug
QT += testlib
TARGET = location-benchmark

HEADERS += location-benchmark.h

SOURCES += location-benchmark.cpp
