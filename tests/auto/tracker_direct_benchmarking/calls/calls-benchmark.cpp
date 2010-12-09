/*
 * Copyright (C) 2010 Nokia Corporation.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#include <calls-benchmark.h>

CallsBenchmark::CallsBenchmark()
{
    ;
}

void CallsBenchmark::initTestCase()
{
    // For running the test without installing the plugins. Should work in
    // normal and vpath builds.
    QCoreApplication::addLibraryPath("../../../../plugins");
}

void CallsBenchmark::test_calls_01()
{
    QBENCHMARK {
        QSparqlConnection conn("QTRACKER_DIRECT");
        QSparqlQuery q("SELECT ?duration ?phonenumber WHERE {"
                "?call  a  nmo:Call ;"
                "nmo:duration ?duration ;"
                "nmo:from [a nco:Contact ; nco:hasPhoneNumber ?phonenumber]"
        "} LIMIT 50");
        QSparqlResult* r = conn.exec(q);
        QVERIFY(r != 0);
        QCOMPARE(r->hasError(), false);
        r->waitForFinished();
        QCOMPARE(r->hasError(), false);
        QCOMPARE(r->size(), 50);
    }
}

void CallsBenchmark::test_calls_02()
{
    QBENCHMARK {
        QSparqlConnection conn("QTRACKER_DIRECT");
        QSparqlQuery q("SELECT ?name ?date ?number ?duration "
        "WHERE {"
                "?m a nmo:Call;"
                "nmo:sentDate ?date ;"
                "nmo:duration ?duration;"
                "nmo:to ?contact ."
                "?contact a nco:PersonContact;"
                "nco:hasPhoneNumber ?number . "
                "OPTIONAL {"
                        "?contact a nco:PersonContact ;"
                        "nco:nameFamily ?name"
                "} "
                "FILTER (?duration > 0) ."
        "} ORDER BY desc(?date) LIMIT 50");
        QSparqlResult* r = conn.exec(q);
        QVERIFY(r != 0);
        QCOMPARE(r->hasError(), false);
        r->waitForFinished();
        QCOMPARE(r->hasError(), false);
        QCOMPARE(r->size(), 50);
    }
}

void CallsBenchmark::test_calls_03()
{
    QBENCHMARK {
        QSparqlConnection conn("QTRACKER_DIRECT");
        QSparqlQuery q("SELECT ?name ?date ?number ?duration "
        "WHERE {"
                "?m a nmo:Call;"
                "nmo:receivedDate ?date ;"
                "nmo:duration ?duration;"
                "nmo:from ?contact ."
                "?contact a nco:PersonContact;"
                "nco:hasPhoneNumber ?number . "
                "OPTIONAL { ?contact a nco:PersonContact ; nco:nameFamily ?name} "
                "FILTER (?duration > 0) .}"
        "ORDER BY desc(?date) LIMIT 50");
        QSparqlResult* r = conn.exec(q);
        QVERIFY(r != 0);
        QCOMPARE(r->hasError(), false);
        r->waitForFinished();
        QCOMPARE(r->hasError(), false);
        QCOMPARE(r->size(), 50);
    }
}

void CallsBenchmark::test_calls_04()
{
    QBENCHMARK {
        QSparqlConnection conn("QTRACKER_DIRECT");
        QSparqlQuery q("SELECT ?name ?date ?number ?duration "
                "WHERE {?m a nmo:Call;"
                "nmo:receivedDate ?date ;"
                "nmo:duration ?duration;"
                "nmo:from ?contact ."
                "?contact a nco:PersonContact;"
                "nco:hasPhoneNumber ?number . "
                "OPTIONAL { ?contact a nco:PersonContact ; nco:nameFamily ?name} "
                "FILTER (?duration > 0) ."
                "} ORDER BY desc(?date) LIMIT 50");
        QSparqlResult* r = conn.exec(q);
        QVERIFY(r != 0);
        QCOMPARE(r->hasError(), false);
        r->waitForFinished();
        QCOMPARE(r->hasError(), false);
        QCOMPARE(r->size(), 50);
    }
}

QTEST_MAIN(CallsBenchmark)
