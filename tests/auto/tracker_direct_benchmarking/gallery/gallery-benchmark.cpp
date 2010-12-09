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

#include <gallery-benchmark.h>

GalleryBenchmark::GalleryBenchmark ()
{
    ;
}

void GalleryBenchmark::initTestCase()
{
    // For running the test without installing the plugins. Should work in
    // normal and vpath builds.
    QCoreApplication::addLibraryPath("../../../../plugins");
}

void GalleryBenchmark::test_gallery_01()
{
    QBENCHMARK {
        QSparqlConnection conn("QTRACKER_DIRECT");
        QSparqlQuery q("SELECT ?url ?filename ?modified ?_width ?_height "
  "WHERE { "
        "?media a nfo:Visual; "
        "nie:url ?url; "
        "nfo:fileName ?filename ; "
        "nfo:fileLastModified ?modified . "
        "OPTIONAL    {?media nfo:width ?_width. } "
        "OPTIONAL   { ?media nfo:height ?_height .} "
  "} "
  "ORDER BY ?modified LIMIT 50");
        QSparqlResult* r = conn.exec(q);
        QVERIFY(r != 0);
        QCOMPARE(r->hasError(), false);
        r->waitForFinished();
        QCOMPARE(r->hasError(), false);
        QCOMPARE(r->size(), 50);
    }
}

void GalleryBenchmark::test_gallery_04()
{
    QBENCHMARK {
        QSparqlConnection conn("QTRACKER_DIRECT");
        QSparqlQuery q("SELECT ?url ?filename ?modified "
  "WHERE { "
        "?media a nfo:Visual; "
        "nie:url ?url; "
        "nfo:fileName ?filename ; "
        "nfo:fileLastModified ?modified . "
  "} "
  "ORDER BY ?modified LIMIT 50");
        QSparqlResult* r = conn.exec(q);
        QVERIFY(r != 0);
        QCOMPARE(r->hasError(), false);
        r->waitForFinished();
        QCOMPARE(r->hasError(), false);
        QCOMPARE(r->size(), 50);
    }
}

void GalleryBenchmark::test_gallery_05()
{
    QBENCHMARK {
        QSparqlConnection conn("QTRACKER_DIRECT");
        QSparqlQuery q("SELECT ?url ?filename ?modified ?_width ?_height "
   "WHERE { "
        "?media a nfo:Visual; "
        "nie:url ?url; "
        "nfo:fileName ?filename ; "
        "nfo:fileLastModified ?modified . "
        "OPTIONAL    {?media nfo:width ?_width. } "
        "OPTIONAL   { ?media nfo:height ?_height .} "
   "} "
   "ORDER BY ?modified LIMIT 50");
        QSparqlResult* r = conn.exec(q);
        QVERIFY(r != 0);
        QCOMPARE(r->hasError(), false);
        r->waitForFinished();
        QCOMPARE(r->hasError(), false);
        QCOMPARE(r->size(), 50);
    }
}

/* Time taken for querying all images and videos without OPTIONAL*/
void GalleryBenchmark::test_gallery_06()
{
    QBENCHMARK {
        QSparqlConnection conn("QTRACKER_DIRECT");
        QSparqlQuery q("SELECT ?url ?filename ?modified "
    "WHERE { "
        "?media a nfo:Visual; "
        "nie:url ?url; "
        "nfo:fileName ?filename ; "
        "nfo:fileLastModified ?modified . "
    "} "
    "ORDER BY ?modified LIMIT 50");
        QSparqlResult* r = conn.exec(q);
        QVERIFY(r != 0);
        QCOMPARE(r->hasError(), false);
        r->waitForFinished();
        QCOMPARE(r->hasError(), false);
        QCOMPARE(r->size(), 50);
    }
}

/* Time taken for querying 500 images and videos */
void GalleryBenchmark::test_gallery_03()
{
    QBENCHMARK {
        QSparqlConnection conn("QTRACKER_DIRECT");
        QSparqlQuery q("SELECT ?media "
    "WHERE { "
        "?media a nfo:Visual; "
        "nao:hasTag ?tag . "
        "?tag nao:prefLabel 'TEST' "
    "} LIMIT 50");
        QSparqlResult* r = conn.exec(q);
        QVERIFY(r != 0);
        QCOMPARE(r->hasError(), false);
        r->waitForFinished();
        QCOMPARE(r->hasError(), false);
        QCOMPARE(r->size(), 50);
    }
}

void GalleryBenchmark::test_gallery_07()
{
    QBENCHMARK {
        QSparqlConnection conn("QTRACKER_DIRECT");
        QSparqlQuery q("SELECT ?media "
    "WHERE { "
        "?media a nfo:Visual; "
        "nao:hasTag ?tag . "
        "?tag nao:prefLabel 'TEST' "
    "} LIMIT 50");
        QSparqlResult* r = conn.exec(q);
        QVERIFY(r != 0);
        QCOMPARE(r->hasError(), false);
        r->waitForFinished();
        QCOMPARE(r->hasError(), false);
        QCOMPARE(r->size(), 50);
    }
}

void GalleryBenchmark::test_gallery_02()
{
    QBENCHMARK {
        QSparqlConnection conn("QTRACKER_DIRECT");
        QSparqlQuery q("SELECT ?media WHERE { "
        "?media a nfo:Visual; "
        "nfo:device 'NOKIA' "
  "} LIMIT 50");
        QSparqlResult* r = conn.exec(q);
        QVERIFY(r != 0);
        QCOMPARE(r->hasError(), false);
        r->waitForFinished();
        QCOMPARE(r->hasError(), false);
        QCOMPARE(r->size(), 50);
    }
}

void GalleryBenchmark::test_gallery_08()
{
    QBENCHMARK {
        QSparqlConnection conn("QTRACKER_DIRECT");
        QSparqlQuery q("SELECT ?media WHERE { "
        "?media a nfo:Visual; "
        "nfo:device 'NOKIA' "
  "} LIMIT 50");
        QSparqlResult* r = conn.exec(q);
        QVERIFY(r != 0);
        QCOMPARE(r->hasError(), false);
        r->waitForFinished();
        QCOMPARE(r->hasError(), false);
        QCOMPARE(r->size(), 50);
    }
}

void GalleryBenchmark::test_gallery_09()
{
    QBENCHMARK {
        QSparqlConnection conn("QTRACKER_DIRECT");
        QSparqlQuery q("SELECT ?url ?height ?width ?mime ?camera ?exposuretime ?fnumber ?focallength "
  "WHERE { "
        "?image a nmm:Photo; "
        "nie:url ?url; "
        "nie:mimeType ?mime. "
        "OPTIONAL { ?image nfo:height ?height .} "
        "OPTIONAL { ?image nfo:width  ?width .} "
        "OPTIONAL { ?image nfo:device ?camera .} "
        "OPTIONAL { ?image nmm:exposureTime ?exposuretime .} "
        "OPTIONAL { ?image nmm:fnumber ?fnumber .} "
        "OPTIONAL { ?image nmm:focalLength ?focallength .} "
  "} LIMIT 50");
        QSparqlResult* r = conn.exec(q);
        QVERIFY(r != 0);
        QCOMPARE(r->hasError(), false);
        r->waitForFinished();
        QCOMPARE(r->hasError(), false);
        QCOMPARE(r->size(), 50);
    }
}

void GalleryBenchmark::test_gallery_10()
{
    QBENCHMARK {
        QSparqlConnection conn("QTRACKER_DIRECT");
        QSparqlQuery q("SELECT ?url ?height ?width ?mime ?camera ?exposuretime ?fnumber ?focallength "
    "WHERE { "
         "?image a nmm:Photo; "
         "nie:url ?url; "
         "nie:mimeType ?mime. "
         "OPTIONAL { ?image nfo:height ?height .} "
         "OPTIONAL { ?image nfo:width  ?width .} "
         "OPTIONAL { ?image nfo:device ?camera .} "
         "OPTIONAL { ?image nmm:exposureTime ?exposuretime .} "
         "OPTIONAL { ?image nmm:fnumber ?fnumber .} "
         "OPTIONAL { ?image nmm:focalLength ?focallength .} "
    "} LIMIT 50");
        QSparqlResult* r = conn.exec(q);
        QVERIFY(r != 0);
        QCOMPARE(r->hasError(), false);
        r->waitForFinished();
        QCOMPARE(r->hasError(), false);
        QCOMPARE(r->size(), 50);
    }
}

void GalleryBenchmark::test_gallery_11()
{
    QBENCHMARK {
        QSparqlConnection conn("QTRACKER_DIRECT");
        QSparqlQuery q("SELECT ?url ?filename ?modified ?_width ?_height "
   "WHERE { "
        "{?media a nmm:Photo.} UNION {?media a nmm:Video.} "
        "?media nie:url ?url. "
        "?media nfo:fileName ?filename . "
        "?media nfo:fileLastModified ?modified . "
        "OPTIONAL    {?media nfo:width ?_width. } "
        "OPTIONAL   { ?media nfo:height ?_height .} } "
        "ORDER BY ?modified LIMIT 50");
        QSparqlResult* r = conn.exec(q);
        QVERIFY(r != 0);
        QCOMPARE(r->hasError(), false);
        r->waitForFinished();
        QCOMPARE(r->hasError(), false);
        QCOMPARE(r->size(), 50);
    }
}

void GalleryBenchmark::test_gallery_12()
{
    QBENCHMARK {
        QSparqlConnection conn("QTRACKER_DIRECT");
        QSparqlQuery q("SELECT nie:url(?image) "
           "nfo:height(?image) "
           "nfo:width(?image) "
           "nie:mimeType(?image) "
           "nfo:device(?image) "
           "nmm:exposureTime(?image) "
           "nmm:fnumber(?image) "
           "nmm:focalLength(?image) "
    "WHERE { "
        "?image a nmm:Photo . "
    "} limit 50");
        QSparqlResult* r = conn.exec(q);
        QVERIFY(r != 0);
        QCOMPARE(r->hasError(), false);
        r->waitForFinished();
        QCOMPARE(r->hasError(), false);
        QCOMPARE(r->size(), 50);
    }
}

void GalleryBenchmark::test_gallery_13()
{
    QBENCHMARK {
        QSparqlConnection conn("QTRACKER_DIRECT");
        QSparqlQuery q("SELECT nie:url(?image) "
           "nfo:height(?image) "
           "nfo:width(?image) "
           "nie:mimeType(?image) "
           "nfo:device(?image) "
           "nmm:exposureTime(?image) "
           "nmm:fnumber(?image) "
           "nmm:focalLength(?image) "
    "WHERE { "
        "?image a nmm:Photo . "
    "} limit 50");
        QSparqlResult* r = conn.exec(q);
        QVERIFY(r != 0);
        QCOMPARE(r->hasError(), false);
        r->waitForFinished();
        QCOMPARE(r->hasError(), false);
        QCOMPARE(r->size(), 50);
    }
}

// Main function and entry point
QTEST_MAIN(GalleryBenchmark)
