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

#include <location-benchmark.h>

LocationBenchmark::LocationBenchmark ()
{
    ;
}

void LocationBenchmark::initTestCase()
{
    // For running the test without installing the plugins. Should work in
    // normal and vpath builds.
    QCoreApplication::addLibraryPath("../../../../plugins");
}

void LocationBenchmark::test_location_01 ()
{
    QBENCHMARK {
        QSparqlConnection conn("QTRACKER_DIRECT");
        QSparqlQuery q("SELECT "
  "?urn "
  "?cLat ?cLon ?cAlt ?cRad "
  "?nwLat ?nwLon ?nwAlt "
  "?seLat ?seLon ?seAlt "
  "?country ?district ?city ?street ?postalcode "
  "nie:title(?urn) "
  "nie:description(?urn) "
  "mlo:belongsToCategory(?urn) "
"WHERE { "
  "?urn a mlo:Landmark . "
  "OPTIONAL "
    "{ "
      "?urn mlo:location "
        "[ "
          "a mlo:GeoLocation ; "
          "mlo:asPostalAddress "
            "[ "
              "a nco:PostalAddress ; "
              "nco:country ?country ; "
              "nco:region ?district ; "
              "nco:locality ?city ; "
              "nco:streetAddress ?street ; "
              "nco:postalcode ?postalcode "
            "] "
        "] "
    "} . "
  "OPTIONAL "
    "{ "
      "?urn mlo:location "
        "[ "
          "a mlo:GeoLocation ; "
          "mlo:asBoundingBox "
            "[ "
              "a mlo:GeoBoundingBox ; "
              "mlo:bbNorthWest "
                "[ "
                  "a mlo:GeoPoint ; "
                  "mlo:latitude ?nwLat ; "
                  "mlo:longitude ?nwLon ; "
                  "mlo:altitude ?nwAlt "
                "] ; "
              "mlo:bbSouthEast "
                "[ "
                  "a mlo:GeoPoint ; "
                  "mlo:latitude ?seLat ; "
                  "mlo:longitude ?seLon ; "
                  "mlo:altitude ?seAlt "
                "] "
            "] "
        "] "
    "} . "
  "OPTIONAL "
    "{ "
      "?urn mlo:location "
        "[ "
          "a mlo:GeoLocation ; "
          "mlo:asGeoPoint "
            "[ "
              "a mlo:GeoPoint ; "
              "mlo:latitude ?cLat ; "
              "mlo:longitude ?cLon "
            "] "
        "] "
    "} . "
  "OPTIONAL "
    "{ "
      "?urn mlo:location "
        "[ "
          "a mlo:GeoLocation ; "
          "mlo:asGeoPoint "
            "[ "
              "a mlo:GeoPoint ; "
              "mlo:altitude ?cAlt "
            "] "
        "] "
    "} . "
  "OPTIONAL "
    "{ "
      "?urn mlo:location "
        "[ "
          "a mlo:GeoLocation ; "
          "mlo:asGeoPoint "
            "[ "
              "a mlo:GeoPoint ; "
              "mlo:radius ?cRad "
            "] "
        "] "
    "} "
"} ORDER BY ASC(?name) LIMIT 50 ");
        QSparqlResult* r = conn.exec(q);
        QVERIFY(r != 0);
        QCOMPARE(r->hasError(), false);
        r->waitForFinished();
        QCOMPARE(r->hasError(), false);
        QCOMPARE(r->size(), 50);
    }
}

void LocationBenchmark::test_location_02 ()
{
    QBENCHMARK {
        QSparqlConnection conn("QTRACKER_DIRECT");
        QSparqlQuery q("SELECT "
  "?urn "
  "?cLat ?cLon ?cAlt ?cRad "
  "?nwLat ?nwLon ?nwAlt "
  "?seLat ?seLon ?seAlt "
  "?country ?district ?city ?street ?postalcode "
  "nie:title(?urn) "
  "nie:description(?urn) "
  "mlo:belongsToCategory(?urn) "
"WHERE { "
  "?urn a mlo:Landmark . "
  "OPTIONAL "
    "{ "
      "?urn mlo:location "
        "[ "
          "a mlo:GeoLocation ; "
          "mlo:asPostalAddress "
            "[ "
              "a nco:PostalAddress ; "
              "nco:country ?country ; "
              "nco:region ?district ; "
              "nco:locality ?city ; "
              "nco:streetAddress ?street ; "
              "nco:postalcode ?postalcode "
            "] "
        "] "
    "} . "
  "OPTIONAL "
    "{ "
      "?urn mlo:location "
        "[ "
          "a mlo:GeoLocation ; "
          "mlo:asBoundingBox "
            "[ "
              "a mlo:GeoBoundingBox ; "
              "mlo:bbNorthWest "
                "[ "
                  "a mlo:GeoPoint ; "
                  "mlo:latitude ?nwLat ; "
                  "mlo:longitude ?nwLon ; "
                  "mlo:altitude ?nwAlt "
                "] ; "
              "mlo:bbSouthEast "
                "[ "
                  "a mlo:GeoPoint ; "
                  "mlo:latitude ?seLat ; "
                  "mlo:longitude ?seLon ; "
                  "mlo:altitude ?seAlt "
                "] "
            "] "
        "] "
    "} . "
  "OPTIONAL "
    "{ "
      "?urn mlo:location "
        "[ "
          "a mlo:GeoLocation ; "
          "mlo:asGeoPoint "
            "[ "
              "a mlo:GeoPoint ; "
              "mlo:latitude ?cLat ; "
              "mlo:longitude ?cLon "
            "] "
        "] "
    "} . "
  "OPTIONAL "
    "{ "
      "?urn mlo:location "
        "[ "
          "a mlo:GeoLocation ; "
          "mlo:asGeoPoint "
            "[ "
              "a mlo:GeoPoint ; "
              "mlo:altitude ?cAlt "
            "] "
        "] "
    "} . "
  "OPTIONAL "
    "{ "
      "?urn mlo:location "
        "[ "
          "a mlo:GeoLocation ; "
          "mlo:asGeoPoint "
            "[ "
              "a mlo:GeoPoint ; "
              "mlo:radius ?cRad "
            "] "
        "] "
    "} "
  "FILTER(?cLat >= 39.16 && ?cLat <= 40.17 && ?cLon >= 63.94 && ?cLon <= 64.96) "
"} ORDER BY ASC(?name) LIMIT 50");
        QSparqlResult* r = conn.exec(q);
        QVERIFY(r != 0);
        QCOMPARE(r->hasError(), false);
        r->waitForFinished();
        QCOMPARE(r->hasError(), false);
        QCOMPARE(r->size(), 50);
    }
}

void LocationBenchmark::test_location_03 ()
{
    QBENCHMARK {
        QSparqlConnection conn("QTRACKER_DIRECT");
        QSparqlQuery q("SELECT "
  "?urn "
  "?cLat ?cLon ?cAlt ?cRad "
  "?nwLat ?nwLon ?nwAlt "
  "?seLat ?seLon ?seAlt "
  "?country ?district ?city ?street ?postalcode "
  "nie:title(?urn) "
  "nie:description(?urn) "
  "mlo:belongsToCategory(?urn) "
  "tracker:haversine-distance(xsd:double(?cLat), xsd:double(39.50), xsd:double(?cLon), xsd:double(64.50)) as ?distance "
"WHERE { "
  "?urn a mlo:Landmark . "
  "OPTIONAL "
    "{ "
      "?urn mlo:location "
        "[ "
          "a mlo:GeoLocation ; "
          "mlo:asPostalAddress "
            "[ "
              "a nco:PostalAddress ; "
              "nco:country ?country ; "
              "nco:region ?district ; "
              "nco:locality ?city ; "
              "nco:streetAddress ?street ; "
              "nco:postalcode ?postalcode "
            "] "
        "] "
    "} . "
  "OPTIONAL "
    "{ "
      "?urn mlo:location "
        "[ "
          "a mlo:GeoLocation ; "
          "mlo:asBoundingBox "
            "[ "
              "a mlo:GeoBoundingBox ; "
              "mlo:bbNorthWest "
                "[ "
                  "a mlo:GeoPoint ; "
                  "mlo:latitude ?nwLat ; "
                  "mlo:longitude ?nwLon ; "
                  "mlo:altitude ?nwAlt "
                "] ; "
              "mlo:bbSouthEast "
                "[ "
                  "a mlo:GeoPoint ; "
                  "mlo:latitude ?seLat ; "
                  "mlo:longitude ?seLon ; "
                  "mlo:altitude ?seAlt "
                "] "
            "] "
        "] "
    "} . "
  "OPTIONAL "
    "{ "
      "?urn mlo:location "
        "[ "
          "a mlo:GeoLocation ; "
          "mlo:asGeoPoint "
            "[ "
              "a mlo:GeoPoint ; "
              "mlo:latitude ?cLat ; "
              "mlo:longitude ?cLon "
            "] "
        "] "
    "} . "
  "OPTIONAL "
    "{ "
      "?urn mlo:location "
        "[ "
          "a mlo:GeoLocation ; "
          "mlo:asGeoPoint "
            "[ "
              "a mlo:GeoPoint ; "
              "mlo:altitude ?cAlt "
            "] "
        "] "
    "} . "
  "OPTIONAL "
    "{ "
      "?urn mlo:location "
        "[ "
          "a mlo:GeoLocation ; "
          "mlo:asGeoPoint "
            "[ "
              "a mlo:GeoPoint ; "
              "mlo:radius ?cRad "
            "] "
        "] "
    "} "
  "FILTER(?cLat >= 39.16 && ?cLat <= 40.17 && "
         "?cLon >= 63.94 && ?cLon <= 64.96 && "
  "  tracker:haversine-distance(xsd:double(?cLat), xsd:double(39.50), xsd:double(?cLon), xsd:double(64.50)) <= 25000) "
"} ORDER BY ASC(?distance) LIMIT 50 ");
        QSparqlResult* r = conn.exec(q);
        QVERIFY(r != 0);
        QCOMPARE(r->hasError(), false);
        r->waitForFinished();
        QCOMPARE(r->hasError(), false);
        QCOMPARE(r->size(), 50);
    }
}

void LocationBenchmark::test_location_04 ()
{
    QBENCHMARK {
        QSparqlConnection conn("QTRACKER_DIRECT");
        QSparqlQuery q("SELECT "
  "?urn "
  "?cLat ?cLon ?cAlt ?cRad "
  "?nwLat ?nwLon ?nwAlt "
  "?seLat ?seLon ?seAlt "
  "?country ?district ?city ?street ?postalcode "
  "nie:title(?urn) "
  "nie:description(?urn) "
  "mlo:belongsToCategory(?urn) "
  "tracker:haversine-distance(xsd:double(?cLat), xsd:double(39.50), xsd:double(?cLon), xsd:double(64.50)) as ?distance "
"WHERE { "
  "?urn a mlo:Landmark . "
  "OPTIONAL "
    "{ "
      "?urn mlo:location "
        "[ "
          "a mlo:GeoLocation ; "
          "mlo:asPostalAddress "
            "[ "
              "a nco:PostalAddress ; "
              "nco:country ?country ; "
              "nco:region ?district ; "
              "nco:locality ?city ; "
              "nco:streetAddress ?street ; "
              "nco:postalcode ?postalcode "
            "] "
        "] "
    "} . "
  "OPTIONAL "
    "{ "
      "?urn mlo:location "
        "[ "
          "a mlo:GeoLocation ; "
          "mlo:asBoundingBox "
            "[ "
              "a mlo:GeoBoundingBox ; "
              "mlo:bbNorthWest "
                "[ "
                  "a mlo:GeoPoint ; "
                  "mlo:latitude ?nwLat ; "
                  "mlo:longitude ?nwLon ; "
                  "mlo:altitude ?nwAlt "
                "] ; "
              "mlo:bbSouthEast "
                "[ "
                  "a mlo:GeoPoint ; "
                  "mlo:latitude ?seLat ; "
                  "mlo:longitude ?seLon ; "
                  "mlo:altitude ?seAlt "
                "] "
            "] "
        "] "
    "} . "
  "OPTIONAL "
    "{ "
      "?urn mlo:location "
        "[ "
          "a mlo:GeoLocation ; "
          "mlo:asGeoPoint "
            "[ "
              "a mlo:GeoPoint ; "
              "mlo:latitude ?cLat ; "
              "mlo:longitude ?cLon "
            "] "
        "] "
    "} . "
  "OPTIONAL "
    "{ "
      "?urn mlo:location "
        "[ "
          "a mlo:GeoLocation ; "
          "mlo:asGeoPoint "
            "[ "
              "a mlo:GeoPoint ; "
              "mlo:altitude ?cAlt "
            "] "
        "] "
    "} . "
  "OPTIONAL "
    "{ "
      "?urn mlo:location "
        "[ "
          "a mlo:GeoLocation ; "
          "mlo:asGeoPoint "
            "[ "
              "a mlo:GeoPoint ; "
              "mlo:radius ?cRad "
            "] "
        "] "
    "} "
  "FILTER(tracker:haversine-distance(xsd:double(?cLat), xsd:double(39.50), xsd:double(?cLon), xsd:double(64.50)) <= 25000) "
"} ORDER BY ASC(?distance) LIMIT 50 ");
        QSparqlResult* r = conn.exec(q);
        QVERIFY(r != 0);
        QCOMPARE(r->hasError(), false);
        r->waitForFinished();
        QCOMPARE(r->hasError(), false);
        QCOMPARE(r->size(), 50);
    }
}

void LocationBenchmark::test_location_05 ()
{
    QBENCHMARK {
        QSparqlConnection conn("QTRACKER_DIRECT");
        QSparqlQuery q("SELECT "
  "?urn "
  "mlo:latitude(?point) mlo:longitude(?point) mlo:altitude(?point) mlo:radius(?point) "
  "nie:title(?urn) "
  "nie:description(?urn) "
  "mlo:belongsToCategory(?urn) "
"WHERE { "
  "?urn a mlo:Landmark . "
  "?urn mlo:location ?location . "
  "?location mlo:asGeoPoint ?point . "
"} ORDER BY ASC(?name) LIMIT 50 ");
        QSparqlResult* r = conn.exec(q);
        QVERIFY(r != 0);
        QCOMPARE(r->hasError(), false);
        r->waitForFinished();
        QCOMPARE(r->hasError(), false);
        QCOMPARE(r->size(), 50);
    }
}

void LocationBenchmark::test_location_06 ()
{
    QBENCHMARK {
        QSparqlConnection conn("QTRACKER_DIRECT");
        QSparqlQuery q("SELECT "
  "?urn "
  "?cLat ?cLon mlo:altitude(?point) mlo:radius(?point) "
  "nie:title(?urn) "
  "nie:description(?urn) "
  "mlo:belongsToCategory(?urn) "
"WHERE { "
  "?urn a mlo:Landmark . "
  "?urn mlo:location ?location . "
  "?location mlo:asGeoPoint ?point . "
  "?point mlo:latitude ?cLat . "
  "?point mlo:longitude ?cLon . "
  "FILTER(?cLat >= 39.16 && ?cLat <= 40.17 && ?cLon >= 63.42 && ?cLon <= 64.96) "
"} ORDER BY ASC(?name) LIMIT 50 ");
        QSparqlResult* r = conn.exec(q);
        QVERIFY(r != 0);
        QCOMPARE(r->hasError(), false);
        r->waitForFinished();
        QCOMPARE(r->hasError(), false);
        QCOMPARE(r->size(), 50);
    }
}

void LocationBenchmark::test_location_07 ()
{
    QBENCHMARK {
        QSparqlConnection conn("QTRACKER_DIRECT");
        QSparqlQuery q("SELECT "
  "?urn "
  "?cLat ?cLon mlo:altitude(?point) mlo:radius(?point) "
  "nie:title(?urn) "
  "nie:description(?urn) "
  "mlo:belongsToCategory(?urn) "
  "tracker:haversine-distance(xsd:double(?cLat), xsd:double(39.50), xsd:double(?cLon), xsd:double(64.50)) as ?distance "
"WHERE { "
  "?urn a mlo:Landmark . "
  "?urn mlo:location ?location . "
  "?location mlo:asGeoPoint ?point . "
  "?point mlo:latitude ?cLat . "
  "?point mlo:longitude ?cLon . "
  "FILTER(?cLat >= 39.16 && ?cLat <= 40.17 && "
         "?cLon >= 63.94 && ?cLon <= 64.96 && "
  "  tracker:haversine-distance(xsd:double(?cLat), xsd:double(39.50), xsd:double(?cLon), xsd:double(64.50)) <= 25000) "
"} ORDER BY ASC(?distance) LIMIT 50 ");
        QSparqlResult* r = conn.exec(q);
        QVERIFY(r != 0);
        QCOMPARE(r->hasError(), false);
        r->waitForFinished();
        QCOMPARE(r->hasError(), false);
        QCOMPARE(r->size(), 50);
    }
}

void LocationBenchmark::test_location_08 ()
{
    QBENCHMARK {
        QSparqlConnection conn("QTRACKER_DIRECT");
        QSparqlQuery q("SELECT "
  "?urn "
  "?cLat ?cLon mlo:altitude(?point) mlo:radius(?point) "
  "nie:title(?urn) "
  "nie:description(?urn) "
  "mlo:belongsToCategory(?urn) "
  "tracker:haversine-distance(xsd:double(?cLat), xsd:double(39.50), xsd:double(?cLon), xsd:double(64.50)) as ?distance "
"WHERE { "
  "?urn a mlo:Landmark . "
  "?urn mlo:location ?location . "
  "?location mlo:asGeoPoint ?point . "
  "?point mlo:latitude ?cLat . "
  "?point mlo:longitude ?cLon . "
  "FILTER(tracker:haversine-distance(xsd:double(?cLat), xsd:double(39.50), xsd:double(?cLon), xsd:double(64.50)) <= 25000) "
"} ORDER BY ASC(?distance) LIMIT 50 ");
        QSparqlResult* r = conn.exec(q);
        QVERIFY(r != 0);
        QCOMPARE(r->hasError(), false);
        r->waitForFinished();
        QCOMPARE(r->hasError(), false);
        QCOMPARE(r->size(), 50);
    }
}

QTEST_MAIN(LocationBenchmark)
