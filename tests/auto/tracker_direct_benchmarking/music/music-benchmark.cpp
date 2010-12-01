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
        QSparqlQuery q("SELECT ?title ?artist nmm:albumTitle (nmm:musicAlbum (?song))"
        "WHERE { {"
                "?song a nmm:MusicPiece ."
                "?song nie:title ?title ."
                "?song nmm:performer ?perf ."
                "?perf nmm:artistName ?artist ."
                "}"
        "}"
        "ORDER BY ?title");
        QSparqlResult* r = conn.exec(q);
        QVERIFY(r != 0);
        QCOMPARE(r->hasError(), false);
        r->waitForFinished();
        QCOMPARE(r->hasError(), false);
        QCOMPARE(r->size(), 50);
    }

    QBENCHMARK {
        QSparqlConnection conn("QTRACKER_DIRECT");
        QSparqlQuery q("SELECT ?title ?artist nmm:albumTitle (nmm:musicAlbum (?song))"
        "WHERE { { "
                        "?song a nmm:MusicPiece . "
                        "?song nie:title ?title ."
                        "?song nmm:performer ?perf . "
                        "?perf nmm:artistName ?artist ."
        "}}  "
                        "ORDER BY ?title ");
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
                "?Songs a nmm:MusicPiece ."
                "?Songs nmm:musicAlbum ?album ."
                "}"
        "}GROUP BY ?album ORDER BY ?album LIMIT 5000");
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
        QSparqlQuery q("SELECT nmm:artistName(?artist) nmm:albumTitle(?album) COUNT(?album) ?artist WHERE {"
                "?song a nmm:MusicPiece ."
                "?song nmm:performer ?artist ."
                "OPTIONAL { ?song nmm:musicAlbum ?album . } "
        "}"
        "GROUP BY ?artist ORDER BY ?artist LIMIT 5000");
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
        QSparqlQuery q("SELECT ?artist ?name COUNT(?album) COUNT (?song)"
        "WHERE {"
                      "?song a nmm:MusicPiece ;"
                      "nmm:musicAlbum ?album;"
                      "nmm:performer ?artist ."
                      "?artist nmm:artistName ?name."
        "} GROUP BY ?artist");
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
        QSparqlQuery q("SELECT nmm:artistName(?artist) COUNT(?songs) WHERE {"
                "?mp a nmm:MusicPiece ."
                "?mp nmm:performer ?artist ;"
                "nie:title ?songs ."
        "}"
        "GROUP BY ?artist ORDER BY DESC(nmm:artistName(?artist))");
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
        QSparqlQuery q("SELECT nie:title(?a) COUNT(?songs) WHERE {"
                "?a a nmm:MusicAlbum ."
                "?mp nmm:musicAlbum ?a ;"
                "nie:title ?songs ."
        "}"
        "GROUP BY ?a ORDER BY DESC(nie:title(?a))");
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
        QSparqlQuery q("SELECT  ?album COUNT(?songs) AS ?count  WHERE {"
                "?a a nmm:MusicAlbum;"
                "nie:title ?album."
                "?mp nmm:musicAlbum ?a;"
                "nie:title ?songs."
        "} GROUP BY ?album ORDER BY DESC(?album)");
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
        QSparqlQuery q("SELECT nmm:artistName(?artist) AS ?artistTitle (nmm:musicAlbum (?song)) (nmm:albumTitle (?album))COUNT(?album) AS ?album ?artist"
        "WHERE { "
                "?song a nmm:MusicPiece  ."
                "?song nmm:performer ?artist ."
        "} GROUP BY ?artist  ORDER BY ?artist LIMIT 5000");
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
        QSparqlQuery q("SELECT nmm:albumTitle(?album) AS ?Album  (nmm:performer(?Songs)) nmm:artistName COUNT(?Songs)  AS ?Songs  ?album"
        "WHERE {"
                "{"
                "?Songs a nmm:MusicPiece ."
                "?Songs nmm:musicAlbum ?album ."
                "}"
        "}GROUP BY ?album ORDER BY ?album LIMIT 100");
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
        QSparqlQuery q("SELECT nmm:artistName(?artist) AS ?artistTitle (nmm:musicAlbum (?song)) nmm:albumTitle COUNT(?album) AS"
                           "?album ?artist"
        "WHERE { "
                "?song a nmm:MusicPiece  ."
                "?song nmm:performer ?artist ."
        "} GROUP BY ?artist  ORDER BY ?artist  LIMIT 100");
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
        QSparqlQuery q("SELECT nmm:artistName(?artist) nmm:albumTitle(?album) COUNT(?album) ?artist WHERE {"
                "?song a nmm:MusicPiece ."
                "?song nmm:performer ?artist ."
                "OPTIONAL  { ?song nmm:musicAlbum ?album . }"
        "}"
        "GROUP BY ?artist ORDER BY ?artist LIMIT 100");
        QSparqlResult* r = conn.exec(q);
        QVERIFY(r != 0);
        QCOMPARE(r->hasError(), false);
        r->waitForFinished();
        QCOMPARE(r->hasError(), false);
        QCOMPARE(r->size(), 50);
    }
}

QTEST_MAIN(MusicBenchmark)
