/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#include <QtTest/QtTest>
#include <QtSparql/QtSparql>

#define NO_QUERIES 100000

class tst_QSparqlTrackerDirectTorture : public QObject
{
    Q_OBJECT

public:
    tst_QSparqlTrackerDirectTorture();
    virtual ~tst_QSparqlTrackerDirectTorture();

public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:
    void syncSelectQueries();
};

tst_QSparqlTrackerDirectTorture::tst_QSparqlTrackerDirectTorture()
{
}

tst_QSparqlTrackerDirectTorture::~tst_QSparqlTrackerDirectTorture()
{
}

void tst_QSparqlTrackerDirectTorture::initTestCase()
{
    // For running the test without installing the plugins. Should work in
    // normal and vpath builds.
    QCoreApplication::addLibraryPath("../../../plugins");
}

void tst_QSparqlTrackerDirectTorture::cleanupTestCase()
{
}

void tst_QSparqlTrackerDirectTorture::init()
{
    cleanup(); // clean leftover test data
    QSparqlConnection conn("QTRACKER_DIRECT");
    QString queryString = "insert { "
        "<qsparql-tracker-direct-tests> a nie:InformationElement .";
    for (int i = 0; i < 100; ++i) {
        queryString += QString("<test-%1> a nie:InformationElement , nie:DataObject ; "
                               "nie:isLogicalPartOf <qsparql-tracker-direct-tests> ; "
                               "nie:byteSize %1 . ").arg(i);
    }
    queryString += "} ";
    QSparqlResult* r = conn.syncExec(QSparqlQuery(queryString, QSparqlQuery::InsertStatement));
    delete r;
    qDebug() << "test data created";
}

void tst_QSparqlTrackerDirectTorture::cleanup()
{
    QSparqlConnection conn("QTRACKER_DIRECT");
    QSparqlQuery q("delete { ?u a rdfs:Resource .} "
                   "where { "
                   "?u nie:isLogicalPartOf <qsparql-tracker-direct-tests> . "
                   "} "
                   "delete { <qsparql-tracker-direct-tests> a rdfs:Resource . }",
                   QSparqlQuery::DeleteStatement);

    QSparqlResult* r = conn.syncExec(q);
    delete r;
}

void tst_QSparqlTrackerDirectTorture::syncSelectQueries()
{
    QSparqlConnection conn("QTRACKER_DIRECT");
    QTest::qWait(2000); // conn opening left out of the torture
    for (int i = 0; i < NO_QUERIES; ++i) {
        if (i % (NO_QUERIES / 10) == 0)
            qDebug() << i << "/" << NO_QUERIES;
        QSparqlQuery q("select ?ie ?bs {"
                       "?ie a nie:InformationElement ; "
                       "nie:isLogicalPartOf <qsparql-tracker-direct-tests> ; "
                       "nie:byteSize ?bs . } order by ?bs");
        QSparqlResult* r = conn.syncExec(q);
        // 3 possibilities: 1) don't call next at all, 2) call next once, 3)
        // call next 10 times

        QList<QVariant> foundResults;
        switch (i % 3) {
        case 0: // fall-through
            for (int j = 0; j < 9; ++j)
            {
                QVERIFY(r->next());
                foundResults << r->value(1);
            }
        case 1: // fall-through
            QVERIFY(r->next());
            foundResults << r->value(1);
        case 2:
            delete r;
        }
        // Check that the foundResults is a subset of all the results
        for (int j = 0; j < foundResults.size(); ++j) {
            QCOMPARE(foundResults[j], QVariant(j));
        }
    }
}

QTEST_MAIN(tst_QSparqlTrackerDirectTorture)
#include "tst_qsparql_tracker_direct_torture.moc"
