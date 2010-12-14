TARGET   = qsparqltrackerdirect

HEADERS     = ../../../../src/sparql/drivers/tracker_direct/qsparql_tracker_direct_p.h
SOURCES     = ../../../../src/plugins/sparqldrivers/tracker_direct/main.cpp \
          ../../../../src/sparql/drivers/tracker_direct/qsparql_tracker_direct.cpp

unix: {
    LIBS *= $$QT_LFLAGS_TRACKER_DIRECT
    QMAKE_CXXFLAGS *= $$QT_CFLAGS_TRACKER_DIRECT -DQSPARQL_LOG_RESULTS
}

include(../../../../src/plugins/sparqldrivers/qsparqldriverbase.pri)

DESTDIR = $$QT_BUILD_TREE/tests/auto/tracker_direct_benchmarking/plugins/sparqldrivers
