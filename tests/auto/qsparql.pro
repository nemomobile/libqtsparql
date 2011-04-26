include(../../shared.pri)

TEMPLATE = subdirs
SUBDIRS = \
    qsparqlquery \
    qsparqlbinding \
    qsparqlresultrow \
    qsparql \
    qsparql_endpoint \
    qsparql_ntriples \
    qsparql_threading \
    qsparql_tracker \
    qsparql_tracker_direct \
    qsparql_tracker_direct_sync \
    qsparql_tracker_direct_crashes \
    qsparql_virtuoso \
    qsparql_virtuoso_endpoint

contains(sparql-plugins, tracker_direct): SUBDIRS += qsparql_benchmark

check.CONFIG = recursive
check.recurse = qsparql qsparqlquery qsparqlbinding qsparql_tracker \
                qsparql_tracker_direct qsparql_tracker_direct_sync qsparql_ntriples

QMAKE_EXTRA_TARGETS += check


testdata.target = qsparql_threading/clean_data_threading.rq

testdata.files = $$testdata.target
testdata.path = $$PREFIX/share/$$PACKAGENAME-tests/
testxml.target = tests.xml
install_testxml.files = $$testxml.target
install_testxml.path = $$PREFIX/share/$$PACKAGENAME-tests/
INSTALLS += target install_testxml testdata
