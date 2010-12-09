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

#include <music-benchmark.h>

MusicBenchmark::MusicBenchmark()
{
    ;
}

void MusicBenchmark::initTestCase()
{
    // For running the test without installing the plugins. Should work in
    // normal and vpath builds.
    QCoreApplication::addLibraryPath("../../../../plugins");
}

void MusicBenchmark::test_audio_01()
{
    QBENCHMARK {
        QSparqlConnection conn("QTRACKER_DIRECT");
        QSparqlQuery q("SELECT ?title ?artist nmm:albumTitle (nmm:musicAlbum (?song)) "
        "WHERE { { "
                "?song a nmm:MusicPiece . "
                "?song nie:title ?title . "
                "?song nmm:performer ?perf . "
                "?perf nmm:artistName ?artist . "
                "} "
        "} "
        "ORDER BY ?title LIMIT 50");
        QSparqlResult* r = conn.exec(q);
        QVERIFY(r != 0);
        QCOMPARE(r->hasError(), false);
        r->waitForFinished();
        QCOMPARE(r->hasError(), false);
        QCOMPARE(r->size(), 50);
    }

    QBENCHMARK {
        QSparqlConnection conn("QTRACKER_DIRECT");
        QSparqlQuery q("SELECT ?title ?artist nmm:albumTitle (nmm:musicAlbum (?song)) "
        "WHERE { { "
                        "?song a nmm:MusicPiece . "
                        "?song nie:title ?title . "
                        "?song nmm:performer ?perf . "
                        "?perf nmm:artistName ?artist . "
        "}}  "
                        "ORDER BY ?title LIMIT 50");
        QSparqlResult* r = conn.exec(q);
        QVERIFY(r != 0);
        QCOMPARE(r->hasError(), false);
        r->waitForFinished();
        QCOMPARE(r->hasError(), false);
        QCOMPARE(r->size(), 50);
    }
}

void MusicBenchmark::test_audio_02()
{
    QBENCHMARK {
        QSparqlConnection conn("QTRACKER_DIRECT");
        QSparqlQuery q("SELECT nmm:albumTitle(?album) AS ?Album  nmm:artistName (nmm:performer (?Songs)) COUNT(?Songs)  AS ?Songs  ?album "
        "WHERE { "
                "{ "
                "?Songs a nmm:MusicPiece . "
                "?Songs nmm:musicAlbum ?album . "
                "} "
        "}GROUP BY ?album ORDER BY ?album LIMIT 50");
        QSparqlResult* r = conn.exec(q);
        QVERIFY(r != 0);
        QCOMPARE(r->hasError(), false);
        r->waitForFinished();
        QCOMPARE(r->hasError(), false);
        QCOMPARE(r->size(), 50);
    }
}

void MusicBenchmark::test_audio_03()
{
    QBENCHMARK {
        QSparqlConnection conn("QTRACKER_DIRECT");
        QSparqlQuery q("SELECT nmm:artistName(?artist) nmm:albumTitle(?album) COUNT(?album) ?artist WHERE { "
                "?song a nmm:MusicPiece . "
                "?song nmm:performer ?artist . "
                "OPTIONAL { ?song nmm:musicAlbum ?album . } "
        "} "
        "GROUP BY ?artist ORDER BY ?artist LIMIT 50");
        QSparqlResult* r = conn.exec(q);
        QVERIFY(r != 0);
        QCOMPARE(r->hasError(), false);
        r->waitForFinished();
        QCOMPARE(r->hasError(), false);
        QCOMPARE(r->size(), 50);
    }
}

void MusicBenchmark::test_audio_04()
{
    QBENCHMARK {
        QSparqlConnection conn("QTRACKER_DIRECT");
        QSparqlQuery q("SELECT ?artist ?name COUNT(?album) COUNT (?song) "
        "WHERE { "
                      "?song a nmm:MusicPiece ; "
                      "nmm:musicAlbum ?album; "
                      "nmm:performer ?artist . "
                      "?artist nmm:artistName ?name. "
        "} GROUP BY ?artist LIMIT 50");
        QSparqlResult* r = conn.exec(q);
        QVERIFY(r != 0);
        QCOMPARE(r->hasError(), false);
        r->waitForFinished();
        QCOMPARE(r->hasError(), false);
        QCOMPARE(r->size(), 50);
    }
}

void MusicBenchmark::test_audio_05()
{
    QBENCHMARK {
        QSparqlConnection conn("QTRACKER_DIRECT");
        QSparqlQuery q("SELECT nmm:artistName(?artist) COUNT(?songs) WHERE { "
                "?mp a nmm:MusicPiece . "
                "?mp nmm:performer ?artist ; "
                "nie:title ?songs . "
        "} "
        "GROUP BY ?artist ORDER BY DESC(nmm:artistName(?artist)) LIMIT 50");
        QSparqlResult* r = conn.exec(q);
        QVERIFY(r != 0);
        QCOMPARE(r->hasError(), false);
        r->waitForFinished();
        QCOMPARE(r->hasError(), false);
        QCOMPARE(r->size(), 50);
    }
}

void MusicBenchmark::test_audio_06()
{
    QBENCHMARK {
        QSparqlConnection conn("QTRACKER_DIRECT");
        QSparqlQuery q("SELECT nie:title(?a) COUNT(?songs) WHERE { "
                "?a a nmm:MusicAlbum . "
                "?mp nmm:musicAlbum ?a ; "
                "nie:title ?songs . "
        "} "
        "GROUP BY ?a ORDER BY DESC(nie:title(?a)) LIMIT 50");
        QSparqlResult* r = conn.exec(q);
        QVERIFY(r != 0);
        QCOMPARE(r->hasError(), false);
        r->waitForFinished();
        QCOMPARE(r->hasError(), false);
        QCOMPARE(r->size(), 50);
    }
}

void MusicBenchmark::test_audio_07()
{
    QBENCHMARK {
        QSparqlConnection conn("QTRACKER_DIRECT");
        QSparqlQuery q("SELECT  ?album COUNT(?songs) AS ?count  WHERE { "
                "?a a nmm:MusicAlbum; "
                "nie:title ?album. "
                "?mp nmm:musicAlbum ?a; "
                "nie:title ?songs. "
        "} GROUP BY ?album ORDER BY DESC(?album) LIMIT 50");
        QSparqlResult* r = conn.exec(q);
        QVERIFY(r != 0);
        QCOMPARE(r->hasError(), false);
        r->waitForFinished();
        QCOMPARE(r->hasError(), false);
        QCOMPARE(r->size(), 50);
    }
}

void MusicBenchmark::test_audio_08()
{
    QBENCHMARK {
        QSparqlConnection conn("QTRACKER_DIRECT");
        QSparqlQuery q("SELECT nmm:artistName(?artist) AS ?artistTitle (nmm:musicAlbum (?song)) (nmm:albumTitle (?album))COUNT(?album) AS ?album ?artist "
        "WHERE { "
                "?song a nmm:MusicPiece  . "
                "?song nmm:performer ?artist . "
        "} GROUP BY ?artist  ORDER BY ?artist LIMIT 50");
        QSparqlResult* r = conn.exec(q);
        QVERIFY(r != 0);
        QCOMPARE(r->hasError(), false);
        r->waitForFinished();
        QCOMPARE(r->hasError(), false);
        QCOMPARE(r->size(), 50);
    }
}

void MusicBenchmark::test_audio_09()
{
    QBENCHMARK {
        QSparqlConnection conn("QTRACKER_DIRECT");
        QSparqlQuery q("SELECT nmm:albumTitle(?album) AS ?Album  (nmm:performer(?Songs)) nmm:artistName COUNT(?Songs)  AS ?Songs  ?album "
        "WHERE { "
                "{ "
                "?Songs a nmm:MusicPiece . "
                "?Songs nmm:musicAlbum ?album . "
                "} "
        "}GROUP BY ?album ORDER BY ?album LIMIT 50");
        QSparqlResult* r = conn.exec(q);
        QVERIFY(r != 0);
        QCOMPARE(r->hasError(), false);
        r->waitForFinished();
        QCOMPARE(r->hasError(), false);
        QCOMPARE(r->size(), 50);
    }
}

void MusicBenchmark::test_audio_10()
{
    QBENCHMARK {
        QSparqlConnection conn("QTRACKER_DIRECT");
        QSparqlQuery q("SELECT nmm:artistName(?artist) AS ?artistTitle (nmm:musicAlbum (?song)) nmm:albumTitle COUNT(?album) AS "
                           "?album ?artist "
        "WHERE { "
                "?song a nmm:MusicPiece  . "
                "?song nmm:performer ?artist . "
        "} GROUP BY ?artist  ORDER BY ?artist  LIMIT 50");
        QSparqlResult* r = conn.exec(q);
        QVERIFY(r != 0);
        QCOMPARE(r->hasError(), false);
        r->waitForFinished();
        QCOMPARE(r->hasError(), false);
        QCOMPARE(r->size(), 50);
    }
}

void MusicBenchmark::test_audio_11()
{
    QBENCHMARK {
        QSparqlConnection conn("QTRACKER_DIRECT");
        QSparqlQuery q("SELECT nmm:artistName(?artist) nmm:albumTitle(?album) COUNT(?album) ?artist WHERE { "
                "?song a nmm:MusicPiece . "
                "?song nmm:performer ?artist . "
                "OPTIONAL  { ?song nmm:musicAlbum ?album . } "
        "} "
        "GROUP BY ?artist ORDER BY ?artist LIMIT 50");
        QSparqlResult* r = conn.exec(q);
        QVERIFY(r != 0);
        QCOMPARE(r->hasError(), false);
        r->waitForFinished();
        QCOMPARE(r->hasError(), false);
        QCOMPARE(r->size(), 50);
    }
}

QTEST_MAIN(MusicBenchmark)
