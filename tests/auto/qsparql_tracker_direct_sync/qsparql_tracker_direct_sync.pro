include(../sparqltest.pri)
CONFIG += qt warn_on console depend_includepath
QT += testlib dbus
HEADERS += ../tracker_direct_common.h ../trackerchangedsignalreceiver.h
SOURCES  += tst_qsparql_tracker_direct_sync.cpp ../tracker_direct_common.cpp ../trackerchangedsignalreceiver.cpp

check.depends = $$TARGET
check.commands = ./tst_qsparql_tracker_direct_sync

QMAKE_EXTRA_TARGETS += check

#QT = sparql # enable this later
