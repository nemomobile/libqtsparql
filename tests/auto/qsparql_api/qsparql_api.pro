include(../sparqltest.pri)
CONFIG += qt warn_on console depend_includepath
QT += testlib xml network
HEADERS += EndpointService.h \
           EndpointServer.h
SOURCES  += tst_qsparql_api.cpp \
            EndpointService.cpp \
            EndpointServer.cpp

check.depends = $$TARGET
check.commands = ./tst_qsparql_api

memcheck.depends = $$TARGET
memcheck.commands = $$VALGRIND $$VALGRIND_OPT ./tst_qsparql_api

QMAKE_EXTRA_TARGETS += check memcheck

#QT = sparql # enable this later
