TEMPLATE = lib

TARGET = sparql-tracker-0.9

HEADERS += dummy_libtracker_sparql.h

SOURCES += dummy_libtracker_sparql.cpp

unix: {
    LIBS *= $$QT_LFLAGS_TRACKER_DIRECT
    QMAKE_CXXFLAGS *= $$QT_CFLAGS_TRACKER_DIRECT
}
