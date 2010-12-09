/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (ivan.frade@nokia.com)
**
** This file is part of the test suite of the QtSparql module (not yet part of the Qt Toolkit).
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at ivan.frade@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

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
