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

#include <content-benchmark.h>

ContentBenchmark::ContentBenchmark ()
{
}

void ContentBenchmark::initTestCase()
{
    // For running the test without installing the plugins. Should work in
    // normal and vpath builds.
    QCoreApplication::addLibraryPath("../../../../plugins");
}

void ContentBenchmark::test_cm_01 ()
{
        /*Get all the contacts that match fts and get relevant UI info for them*/

    QBENCHMARK {
        QSparqlConnection conn("QTRACKER_DIRECT");
        QSparqlQuery q("SELECT DISTINCT ?url ?photourl ?imstatus tracker:coalesce(?family, ?given, ?orgname, ?nick, ?email, ?phone, ?blog) "
        "WHERE { "
        "{ ?url a nco:PersonContact.?url fts:match 'fami*'. } "
        "UNION "
        "{ ?url a nco:PersonContact. ?url nco:hasEmailAddress ?add. ?add fts:match 'fami*'. } "
        "UNION "
        "{ ?url a nco:PersonContact. ?url nco:hasPostalAddress ?post. ?post fts:match 'fami*'. } "
        "OPTIONAL { ?url maemo:relevance ?relevance. } "
        "OPTIONAL { ?url nco:photo ?photo. ?photo nie:url ?photourl. } "
        "OPTIONAL { ?url nco:imContactStatusMessage ?imstatus. } "
        "OPTIONAL { ?url nco:nameFamily ?family. } "
        "OPTIONAL { ?url nco:nameFamily ?given. } "
        "OPTIONAL { ?url nco:org ?org. ?org nco:nameGiven ?orgname. } "
        "OPTIONAL { ?url nco:hasIMAccount ?acc. ?acc nco:imNickname ?nick. } "
        "OPTIONAL { ?url nco:hasEmailAddress ?hasemail. ?hasemail nco:emailAddress ?email. } "
        "OPTIONAL { ?url nco:hasPhoneNumber ?hasphone. ?hasphone nco:phoneNumber ?phone. } "
        "OPTIONAL { ?url nco:blogUrl ?blog. } "
        "} "
        "ORDER BY ?relevance "
        "LIMIT 100");
        QSparqlResult* r = conn.exec(q);
        QVERIFY(r != 0);
        QCOMPARE(r->hasError(), false);
        r->waitForFinished();
        QCOMPARE(r->hasError(), false);
        QCOMPARE(r->size(), 50);
    }
}

void ContentBenchmark::test_cm_02 ()
{
        /*Get all the contacts that match fts and get relevant UI info for them*/

    QBENCHMARK {
        QSparqlConnection conn("QTRACKER_DIRECT");
        QSparqlQuery q("SELECT DISTINCT ?url tracker:coalesce(nco:nameFamily(?url), nco:nameGiven(?url), 'unknown') "
        "WHERE { "
        "{ ?url a nco:PersonContact.?url fts:match 'fami*'. } "
        "UNION { ?url a nco:PersonContact. ?url nco:hasEmailAddress ?add. ?add fts:match 'fami*'. } "
        "UNION { ?url a nco:PersonContact. ?url nco:hasPostalAddress ?post. ?post fts:match 'fami*'. } "
        "} LIMIT 100");
        QSparqlResult* r = conn.exec(q);
        QVERIFY(r != 0);
        QCOMPARE(r->hasError(), false);
        r->waitForFinished();
        QCOMPARE(r->hasError(), false);
        QCOMPARE(r->size(), 50);
    }
}

void ContentBenchmark::test_cm_03 ()
{
        /*Get all the messages */

    QBENCHMARK {
        QSparqlConnection conn("QTRACKER_DIRECT");
        QSparqlQuery q("SELECT DISTINCT ?url nie:title(?url) "
        "WHERE { "
        "{ ?url a nmo:Message. ?url fts:match 'fami*'. } "
        "UNION { ?url a nmo:Message. ?url nmo:from ?from . ?from fts:match 'fami*'. } "
        "UNION { ?url a nmo:Message. ?url nmo:recipient ?to . ?to fts:match 'fami*'. } "
        "} LIMIT 100");
        QSparqlResult* r = conn.exec(q);
        QVERIFY(r != 0);
        QCOMPARE(r->hasError(), false);
        r->waitForFinished();
        QCOMPARE(r->hasError(), false);
        QCOMPARE(r->size(), 50);
    }
}

void ContentBenchmark::test_cm_04 ()
{
        /*Get all the messages */

    QBENCHMARK {
        QSparqlConnection conn("QTRACKER_DIRECT");
        QSparqlQuery q("SELECT ?url ?fileLastModified ?relevance ?fileName ?mimeType ?url2 "
                "WHERE { "
                "?url a nfo:Image . "
                "?url nfo:fileLastModified ?fileLastModified. "
                "?url nfo:fileName ?fileName . "
                "?url nie:mimeType ?mimeType . "
                "?url nie:url ?url2 . "
                "OPTIONAL { ?url maemo:relevance ?relevance. } "
                "} ORDER BY ?_fileName");
        QSparqlResult* r = conn.exec(q);
        QVERIFY(r != 0);
        QCOMPARE(r->hasError(), false);
        r->waitForFinished();
        QCOMPARE(r->hasError(), false);
        QCOMPARE(r->size(), 50);
    }
}

void ContentBenchmark::test_cm_05 ()
{
        /*Get all the matching data */
        /*
        ::tracker()->setVerbosity (2);
        QBENCHMARK {
                RDFSelect query;
                query.distinct (true);

                RDFVariable glob_url ("glob_url");

                RDFVariableList alternatives = glob_url.unionChildren (2);

                RDFSubSelect ua_query;
                RDFVariable ua_url = ua_query.variable (alternatives[0]);

                ua_url.isOfType <nmo::Message> ();
                ua_url.property (fts::iri ("match")).merge (LiteralValue ("fami*"));

                ua_query.addColumn (ua_url);


                RDFSubSelect ub_query;
                RDFVariable ub_url = ub_query.variable (alternatives[1]);
                RDFVariable ub_from ("from");

                ub_url.isOfType <nmo::Message> ();
                ub_url.property <nmo::from> (ub_from);
                ub_from.property (fts::iri ("match")).merge (LiteralValue ("fami*"));

                ub_query.addColumn (ub_url);

                query.addColumn (glob_url);
                query.limit (100);

                LiveNodes query_model = ::tracker()->modelQuery (query, SopranoLive::RDFStrategy::LiveStrategy);
                if (query_model->rowCount() != 50) {
                        QWARN (qPrintable(QString("Unexpected number of results. Expected = %1, Actual = %2").arg(50).arg(query_model->rowCount())));
                }
        }
        */
    QBENCHMARK {
        QSparqlConnection conn("QTRACKER_DIRECT");
        QSparqlQuery q("SELECT DISTINCT ?glob_url "
                "WHERE "
                "{ "
                  "{ SELECT ?url as ?glob_url "
                    "WHERE { ?url a nmo:Message . "
                     "?url fts:match 'fami*' . "
                     "?url nmo:from ?from . } } "
                  "UNION "
                  "{ SELECT ?url as ?glob_url "
                    "WHERE { ?url a nmo:Message . "
                                     "?url nmo:from ?from . "
                                     "?from fts:match 'fami*'. } } "
                  "UNION "
                  "{ SELECT ?url as ?glob_url "
                    "WHERE { ?url a nmo:Message . "
                     "?url nmo:to ?to . "
                     "?to fts:match 'fami*' . } } "
                  "UNION "
                  "{ SELECT ?url as ?glob_url "
                    "WHERE { ?url a nmo:Message. "
                     "?url nmo:communicationChannel ?cha . "
                     "?cha fts:match 'fami*'. } } "
                  "UNION "
                  "{ SELECT ?url as ?glob_url "
                    "WHERE { ?url a nco:PersonContact . "
                     "?url fts:match 'fami*'. } } "
                  "UNION "
                  "{ SELECT ?url as ?glob_url "
                    "WHERE { ?url a nco:PersonContact . "
                     "?url nco:hasEmailAddress ?email . "
                     "?email fts:match 'fami*'. } } "
                  "UNION "
                  "{ SELECT ?url as ?glob_url "
                    "WHERE { ?url a nco:PersonContact . "
                     "?url nco:hasPostalAddress ?post . "
                     "?post fts:match 'fami*'. } } "
                  "UNION "
                  "{ SELECT ?url as ?glob_url "
                    "WHERE { ?url a nmm:MusicPiece . "
                     "?url nmm:performer ?artist . "
                     "?artist fts:match 'fami*' . } } "
                  "UNION "
                  "{ SELECT ?url as ?glob_url "
                    "WHERE { ?url a nmm:MusicPiece . "
                     "?url nmm:musicAlbum ?album . "
                     "?album fts:match 'fami*' . } } "
                  "} "
                "LIMIT 100");
        QSparqlResult* r = conn.exec(q);
        QVERIFY(r != 0);
        QCOMPARE(r->hasError(), false);
        r->waitForFinished();
        QCOMPARE(r->hasError(), false);
        QCOMPARE(r->size(), 50);
    }
}

void ContentBenchmark::test_cm_06 ()
{
        /*Get all the matching data */

    QBENCHMARK {
        QSparqlConnection conn("QTRACKER_DIRECT");
        QSparqlQuery q("SELECT DISTINCT ?glob_url ?first ?second "
                "WHERE "
                "{ "
                  "{ SELECT ?url as ?glob_url "
                    "nmo:messageSubject(?url) as ?first "
                    "tracker:coalesce(nco:fullname(?from), nco:nameGiven(?from), nco:nameFamily(?from), nco:org(?from), 'unknown') as ?second "
                    "WHERE { ?url a nmo:Message . "
                     "?url fts:match 'fami*' . "
                     "?url nmo:from ?from . } } "
                  "UNION "
                  "{ SELECT ?url as ?glob_url "
                    "nmo:messageSubject(?url) as ?first "
                    "tracker:coalesce(nco:fullname(?from), nco:nameGiven(?from), nco:nameFamily(?from), nco:org(?from), 'unknown') as ?second "
                    "WHERE { ?url a nmo:Message . "
                                     "?url nmo:from ?from . "
                                     "?from fts:match 'fami*'. } } "
                  "UNION "
                  "{ SELECT ?url as ?glob_url "
                    "nmo:messageSubject(?url) as ?first "
                    "tracker:coalesce(nco:fullname(?from), nco:nameGiven(?from), nco:nameFamily(?from), nco:org(?from), 'unknown') as ?second "
                    "WHERE { ?url a nmo:Message . "
                     "?url nmo:to ?to . "
                     "?to fts:match 'fami*' . "
                     "?url nmo:from ?from . } } "
                  "UNION "
                  "{ SELECT ?url as ?glob_url "
                    "nmo:messageSubject(?url) as ?first "
                    "tracker:coalesce(nco:fullname(?from), nco:nameGiven(?from), nco:nameFamily(?from), nco:org(?from), 'unknown') as ?second "
                    "WHERE { ?url a nmo:Message. "
                     "?url nmo:communicationChannel ?cha . "
                     "?cha fts:match 'fami*'. "
                     "?url nmo:from ?from . } } "
                  "UNION "
                  "{ SELECT ?url as ?glob_url "
                    "tracker:coalesce(nco:fullname(?url), nco:nameGiven(?url), nco:nameFamily(?url), nco:org(?url), 'unknown') as ?first "
                    "tracker:coalesce(nco:emailAddress(?email), nco:imNickname(?im), 'unknown') as ?second "
                    "WHERE { ?url a nco:PersonContact . "
                     "?url fts:match 'fami*'. "
                     "{ SELECT ?em as ?email WHERE { ?url nco:hasEmailAddress ?em }  LIMIT 1 } "
                     "{ SELECT ?imadd as ?im WHERE { ?url nco:hasIMAddress ?imadd }  LIMIT 1 } } } "
                  "UNION "
                  "{ SELECT ?url as ?glob_url "
                    "tracker:coalesce(nco:fullname(?url), nco:nameGiven(?url), nco:nameFamily(?url), nco:org(?url), 'unknown') as ?first "
                    "tracker:coalesce(nco:emailAddress(?email), nco:imNickname(?im), 'unknown') as ?second "
                    "WHERE { ?url a nco:PersonContact . "
                     "?url nco:hasEmailAddress ?email . "
                     "?email fts:match 'fami*'. "
                     "{ SELECT ?imadd as ?im WHERE { ?url nco:hasIMAddress ?imadd }  LIMIT 1 } } } "
                  "UNION "
                  "{ SELECT ?url as ?glob_url "
                    "tracker:coalesce(nco:fullname(?url), nco:nameGiven(?url), nco:nameFamily(?url), nco:org(?url),'unknown') as ?first "
                    "tracker:coalesce(nco:emailAddress(?email), nco:imNickname(?im), 'unknown') as ?second "
                    "WHERE { ?url a nco:PersonContact . "
                     "?url nco:hasPostalAddress ?post . "
                     "?post fts:match 'fami*'. "
                     "{ SELECT ?em as ?email WHERE { ?url nco:hasEmailAddress ?em }  LIMIT 1 } "
                     "{ SELECT ?imadd as ?im WHERE { ?url nco:hasIMAddress ?imadd }  LIMIT 1 } } } "
                  "UNION "
                  "{ SELECT ?url as ?glob_url "
                    "nie:title(?url) as ?first "
                    "fn:concat(nmm:artistName(?artist),'-',nmm:albumTitle(?album)) "
                    "WHERE { ?url a nmm:MusicPiece . "
                     "?url nmm:performer ?artist . "
                     "?artist fts:match 'fami*' . "
                     "OPTIONAL { ?url nmm:musicAlbum ?album . } } } "
                  "UNION "
                  "{ SELECT ?url as ?glob_url "
                    "nie:title(?url) as ?first "
                    "fn:concat(nmm:artistName(?artist),'-',nmm:albumTitle(?album)) "
                    "WHERE { ?url a nmm:MusicPiece . "
                     "?url nmm:musicAlbum ?album . "
                     "?album fts:match 'fami*' . "
                     "OPTIONAL { ?url nmm:performer ?artist }} } "
                  "} "
                "LIMIT 100");
        QSparqlResult* r = conn.exec(q);
        QVERIFY(r != 0);
        QCOMPARE(r->hasError(), false);
        r->waitForFinished();
        QCOMPARE(r->hasError(), false);
        QCOMPARE(r->size(), 50);
    }
}

QTEST_MAIN(ContentBenchmark)
