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

#include <QtCore/qstringlist.h>
#include <QtTest/QtTest>
#include <QtSparql/QtSparql>

//const QString qtest(qTableName( "qtest", __FILE__ )); // FIXME: what's this

//TESTED_FILES=

class tst_QSparqlTrackerDirect : public QObject
{
    Q_OBJECT

public:
    tst_QSparqlTrackerDirect();
    virtual ~tst_QSparqlTrackerDirect();

public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:
    void query_contacts();
    void qsparqlresultrow();
    void query_forward_only_wait_for_finished();
    void query_contacts_async();
    void query_async_forward_only();
    void query_async_forward_only_large_result_set();
    void query_async_forward_only_large_result_set_data();
    
    void ask_contacts();
    void insert_and_delete_contact();
    void insert_and_delete_contact_async();
    void insert_new_urn();

    void query_with_error();

    void iterate_result();

    void delete_unfinished_result();
    void delete_partially_iterated_result();
    void delete_nearly_finished_result();
    void cancel_insert_result();
    
    void concurrent_queries();
    void concurrent_queries_2();

    void insert_with_dbus_read_with_direct();

    void open_connection_twice();

    void result_type_bool();

    void special_chars();

    void result_immediately_finished();
    void result_immediately_finished2();
    
    void delete_connection_immediately();
    void delete_connection_before_a_wait();

    void go_beyond_columns_number();

    void create_2_connections();

    void unsupported_statement_type();

    void async_conn_opening();
    void async_conn_opening_data();
    void async_conn_opening_with_2_connections();
    void async_conn_opening_with_2_connections_data();
    void async_conn_opening_for_update();
    void async_conn_opening_for_update_data();

    void dataTypes();
    void explicitDataTypes();
    void largeInteger();

    void deleteLaterWithSelectResult();
    void deleteLaterWithUpdateResult();
private:
    bool createTestData(int testDataAmount);
    void cleanupTestData();
};

namespace {
int testLogLevel = QtWarningMsg;
void myMessageOutput(QtMsgType type, const char *msg)
{
    switch (type) {
    case QtDebugMsg:
        if (testLogLevel <= 0)
            fprintf(stderr, "QDEBUG : %s\n", msg);
        break;
    case QtWarningMsg:
        if (testLogLevel <= 1)
            fprintf(stderr, "QWARN  : %s\n", msg);
        break;
    case QtCriticalMsg:
        if (testLogLevel <= 2)
            fprintf(stderr, "QCRITICAL: %s\n", msg);
        break;
    case QtFatalMsg:
        if (testLogLevel <= 3)
            fprintf(stderr, "QFATAL : %s\n", msg);
        abort();
    }
}
} // end unnamed namespace

tst_QSparqlTrackerDirect::tst_QSparqlTrackerDirect()
{
}

tst_QSparqlTrackerDirect::~tst_QSparqlTrackerDirect()
{
}

void tst_QSparqlTrackerDirect::initTestCase()
{
    // For running the test without installing the plugins. Should work in
    // normal and vpath builds.
    QCoreApplication::addLibraryPath("../../../plugins");
    qInstallMsgHandler(myMessageOutput);
    cleanupTestData();
    createTestData(2000);
}

void tst_QSparqlTrackerDirect::cleanupTestCase()
{
    cleanupTestData();
}

void tst_QSparqlTrackerDirect::init()
{
    testLogLevel = QtDebugMsg;
}

void tst_QSparqlTrackerDirect::cleanup()
{
}

void tst_QSparqlTrackerDirect::query_contacts()
{
    QSparqlConnection conn("QTRACKER_DIRECT");
    QSparqlQuery q("select ?u ?ng {?u a nco:PersonContact; "
                   "nie:isLogicalPartOf <qsparql-tracker-direct-tests> ;"
                   "nco:nameGiven ?ng .}");
    QSparqlResult* r = conn.exec(q);
    QVERIFY(r != 0);
    QCOMPARE(r->hasError(), false);
    r->waitForFinished(); // this test is synchronous only
    QCOMPARE(r->hasError(), false);
    QCOMPARE(r->size(), 3);
    QHash<QString, QString> contactNames;
    while (r->next()) {
        QCOMPARE(r->current().count(), 2);
        contactNames[r->value(0).toString()] = r->value(1).toString();
    }
    QCOMPARE(contactNames.size(), 3);
    QCOMPARE(contactNames["uri001"], QString("name001"));
    QCOMPARE(contactNames["uri002"], QString("name002"));
    QCOMPARE(contactNames["uri003"], QString("name003"));
    delete r;
}

void tst_QSparqlTrackerDirect::qsparqlresultrow()
{
    QSparqlConnection conn("QTRACKER_DIRECT");
    QSparqlQuery q("select ?u ?ng {?u a nco:PersonContact; "
                   "nie:isLogicalPartOf <qsparql-tracker-direct-tests> ;"
                   "nco:nameGiven ?ng .}");
    QSparqlResult* r = conn.exec(q);
    QVERIFY(r != 0);
    QCOMPARE(r->hasError(), false);
    r->waitForFinished(); // this test is synchronous only
    QCOMPARE(r->hasError(), false);
    QCOMPARE(r->size(), 3);
    QVERIFY(r->next());
    QSparqlResultRow row = r->current();

    QCOMPARE(row.count(), 2);
    QCOMPARE(row.isEmpty(), false);

    QCOMPARE(row.indexOf("foo"), -1);
    QCOMPARE(row.indexOf("NG"), -1);
    QCOMPARE(row.indexOf(""), -1);
    QCOMPARE(row.indexOf("ng"), 1);

    QCOMPARE(row.contains("foo"), false);
    QCOMPARE(row.contains("NG"), false);
    QCOMPARE(row.contains(""), false);
    QCOMPARE(row.contains("ng"), true);

    QCOMPARE(row.variableName(0), QString::fromLatin1("u"));
    QCOMPARE(row.variableName(1), QString::fromLatin1("ng"));
    QCOMPARE(row.variableName(2), QString());
    QCOMPARE(row.variableName(-1), QString());

    row.clear();
    QCOMPARE(row.isEmpty(), true);
    QCOMPARE(row.contains("ng"), false);
    QCOMPARE(row.variableName(1), QString());
    QCOMPARE(row.indexOf("ng"), -1);

    delete r;
}

void tst_QSparqlTrackerDirect::query_forward_only_wait_for_finished()
{
    QSparqlConnectionOptions opts;
    opts.setForwardOnly();
    QSparqlConnection conn("QTRACKER_DIRECT", opts);
    QSparqlQuery q("select ?u ?ng {?u a nco:PersonContact; "
                   "nie:isLogicalPartOf <qsparql-tracker-direct-tests> ;"
                   "nco:nameGiven ?ng .}");
    QSparqlResult* r = conn.exec(q);
    QVERIFY(r != 0);
    QCOMPARE(r->hasError(), false);
    r->waitForFinished(); // this test is synchronous only
    // Calling waitForFinished() in forward only mode is an error
    QCOMPARE(r->hasError(), true);
}

void tst_QSparqlTrackerDirect::query_contacts_async()
{
    QSparqlConnection conn("QTRACKER_DIRECT");
    QSparqlQuery q("select ?u ?ng {?u a nco:PersonContact; "
                   "nie:isLogicalPartOf <qsparql-tracker-direct-tests> ;"
                   "nco:nameGiven ?ng .}");
    QSparqlResult* r = conn.exec(q);
    QVERIFY(r != 0);
    QCOMPARE(r->hasError(), false);

    QSignalSpy spy(r, SIGNAL(finished()));
    while (spy.count() == 0) {
        QTest::qWait(100);
    }

    QCOMPARE(spy.count(), 1);

    QCOMPARE(r->hasError(), false);
    QCOMPARE(r->size(), 3);
    QHash<QString, QString> contactNames;
    while (r->next()) {
        QCOMPARE(r->current().count(), 2);
        contactNames[r->value(0).toString()] = r->value(1).toString();
    }
    QCOMPARE(contactNames.size(), 3);
    QCOMPARE(contactNames["uri001"], QString("name001"));
    QCOMPARE(contactNames["uri002"], QString("name002"));
    QCOMPARE(contactNames["uri003"], QString("name003"));
    delete r;
}

namespace {
class ForwardOnlyDataReadyListener : public QObject
{
    Q_OBJECT
public:
    ForwardOnlyDataReadyListener(QSparqlResult* r, QStringList* l = 0) : result(r), received(0), list(l)
    {
        connect(r, SIGNAL(dataReady(int)),
                SLOT(onDataReady(int)));
    }
public slots:
    void onDataReady(int tc)
    {
        // qDebug() << "ForwardOnlyDataReadyListener::onDataReady() tc:" << tc << "result->pos():" << result->pos();
        received = tc;
        while (result->next()) {
            if (list != 0)
                list->append(result->stringValue(0));
        }
    }
public:
    QSparqlResult* result;
    int received;
    QStringList* list;
};

}

void tst_QSparqlTrackerDirect::query_async_forward_only()
{
    QSparqlConnectionOptions opts;
    opts.setForwardOnly();
    opts.setDataReadyInterval(1);

    QSparqlConnection conn("QTRACKER_DIRECT", opts);
    // A big query returning a lot of results
    QSparqlQuery q("select ?n {?u a nco:PersonContact; nco:nameGiven ?n ;"
                   "nie:isLogicalPartOf <qsparql-tracker-direct-bulk-tests> .}");

    QSparqlResult* r = conn.exec(q);
    QVERIFY(r != 0);
    QCOMPARE(r->hasError(), false);

    ForwardOnlyDataReadyListener listener(r);
    QTest::qWait(30000);
    QCOMPARE(r->isFinished(), true);
}

void tst_QSparqlTrackerDirect::query_async_forward_only_large_result_set()
{
    QFETCH(int, dataReadyInterval1);
    QFETCH(int, dataReadyInterval2);

    QSparqlConnectionOptions opts1;
    opts1.setForwardOnly();
    opts1.setDataReadyInterval(dataReadyInterval1);

    QSparqlConnection conn1("QTRACKER_DIRECT", opts1);
    // A big query returning a lot of results
    QSparqlQuery q1("select ?n {?u a nco:PersonContact; nco:nameGiven ?n ;"
                   "nie:isLogicalPartOf <qsparql-tracker-direct-bulk-tests> .}");

    QSparqlResult* r1 = conn1.exec(q1);
    QVERIFY(r1 != 0);
    QCOMPARE(r1->hasError(), false);

    QStringList results1;
    ForwardOnlyDataReadyListener listener1(r1, &results1);
    QTest::qWait(30000);
    QCOMPARE(r1->isFinished(), true);
    
    QSparqlConnectionOptions opts2;
    opts2.setDataReadyInterval(dataReadyInterval2);

    QSparqlConnection conn2("QTRACKER_DIRECT", opts2);
    // A big query returning a lot of results
    QSparqlQuery q2("select ?n {?u a nco:PersonContact; nco:nameGiven ?n ;"
                   "nie:isLogicalPartOf <qsparql-tracker-direct-bulk-tests> .}");

    QSparqlResult* r2 = conn2.exec(q2);
    QVERIFY(r2 != 0);
    QCOMPARE(r2->hasError(), false);

    QStringList results2;
    ForwardOnlyDataReadyListener listener2(r2, &results2);
    QTest::qWait(30000);
    QCOMPARE(r2->isFinished(), true);

    QCOMPARE(results1, results2);
}

void tst_QSparqlTrackerDirect::query_async_forward_only_large_result_set_data()
{
    QTest::addColumn<int>("dataReadyInterval1");
    QTest::addColumn<int>("dataReadyInterval2");

    QTest::newRow("DataReadyMinimum")
        << 1 << 1;

    QTest::newRow("DataReadyMaximum")
        << 20 << 20;
}

void tst_QSparqlTrackerDirect::ask_contacts()
{
    QSparqlConnection conn("QTRACKER_DIRECT");
    QSparqlQuery q1("ask {<uri003> a nco:PersonContact; "
                   "nie:isLogicalPartOf <qsparql-tracker-direct-tests> ;"
                   "nco:nameGiven \"name003\" .}", QSparqlQuery::AskStatement);
    QSparqlResult* r = conn.exec(q1);
    QVERIFY(r != 0);
    QCOMPARE(r->hasError(), false);
    r->waitForFinished(); // this test is synchronous only
    QCOMPARE(r->hasError(), false);
    QCOMPARE(r->boolValue(), true);
    delete r;

    QSparqlQuery q2("ask {<uri005> a nco:PersonContact; "
                   "nie:isLogicalPartOf <qsparql-tracker-direct-tests> ;"
                   "nco:nameGiven \"name005\" .}", QSparqlQuery::AskStatement);
    r = conn.exec(q2);
    QVERIFY(r != 0);
    QCOMPARE(r->hasError(), false);
    r->waitForFinished(); // this test is synchronous only
    QCOMPARE(r->hasError(), false);
    QCOMPARE(r->boolValue(), false);
    delete r;
}

void tst_QSparqlTrackerDirect::insert_and_delete_contact()
{
    // This test will leave unclean test data in tracker if it crashes.
    QSparqlConnection conn("QTRACKER_DIRECT");
    QSparqlQuery add("insert { <addeduri001> a nco:PersonContact; "
                     "nie:isLogicalPartOf <qsparql-tracker-direct-tests> ;"
                     "nco:nameGiven \"addedname001\" .}",
                     QSparqlQuery::InsertStatement);

    QSparqlResult* r = conn.exec(add);
    QVERIFY(r != 0);
    QCOMPARE(r->hasError(), false);
    r->waitForFinished(); // this test is synchronous only
    QCOMPARE(r->hasError(), false);
    delete r;

    // Verify that the insertion succeeded
    QSparqlQuery q("select ?u ?ng {?u a nco:PersonContact; "
                   "nie:isLogicalPartOf <qsparql-tracker-direct-tests> ;"
                   "nco:nameGiven ?ng .}");
    QHash<QString, QString> contactNames;
    r = conn.exec(q);
    QVERIFY(r != 0);
    r->waitForFinished();
    QCOMPARE(r->size(), 4);
    while (r->next()) {
        contactNames[r->binding(0).value().toString()] =
            r->binding(1).value().toString();
    }
    QCOMPARE(contactNames.size(), 4);
    QCOMPARE(contactNames["addeduri001"], QString("addedname001"));
    delete r;

    // Delete the uri
    QSparqlQuery del("delete { <addeduri001> a rdfs:Resource. }",
                     QSparqlQuery::DeleteStatement);

    r = conn.exec(del);
    QVERIFY(r != 0);
    QCOMPARE(r->hasError(), false);
    r->waitForFinished(); // this test is synchronous only
    QCOMPARE(r->hasError(), false);
    delete r;

    // Verify that it got deleted
    contactNames.clear();
    r = conn.exec(q);
    QVERIFY(r != 0);
    r->waitForFinished();
    QCOMPARE(r->size(), 3);
    while (r->next()) {
        contactNames[r->binding(0).value().toString()] =
            r->binding(1).value().toString();
    }
    QCOMPARE(contactNames.size(), 3);
    delete r;
}

void tst_QSparqlTrackerDirect::insert_and_delete_contact_async()
{
    // This test will leave unclean test data in tracker if it crashes.
    QSparqlConnection conn("QTRACKER_DIRECT");
    QSparqlQuery add("insert { <addeduri001> a nco:PersonContact; "
                     "nie:isLogicalPartOf <qsparql-tracker-direct-tests> ;"
                     "nco:nameGiven \"addedname001\" .}",
                     QSparqlQuery::InsertStatement);

    QSparqlResult* r = conn.exec(add);
    QVERIFY(r != 0);
    QCOMPARE(r->hasError(), false);

    QSignalSpy insertSpy(r, SIGNAL(finished()));
    while (insertSpy.count() == 0) {
        QTest::qWait(100);
    }
    QCOMPARE(insertSpy.count(), 1);

    QCOMPARE(r->hasError(), false);
    delete r;

    // Verify that the insertion succeeded
    QSparqlQuery q("select ?u ?ng {?u a nco:PersonContact; "
                   "nie:isLogicalPartOf <qsparql-tracker-direct-tests> ;"
                   "nco:nameGiven ?ng .}");
    QHash<QString, QString> contactNames;
    r = conn.exec(q);
    QVERIFY(r != 0);
    r->waitForFinished();
    QCOMPARE(r->size(), 4);
    while (r->next()) {
        contactNames[r->binding(0).value().toString()] =
            r->binding(1).value().toString();
    }
    QCOMPARE(contactNames.size(), 4);
    QCOMPARE(contactNames["addeduri001"], QString("addedname001"));
    delete r;

    // Delete the uri
    QSparqlQuery del("delete { <addeduri001> a rdfs:Resource. }",
                     QSparqlQuery::DeleteStatement);

    r = conn.exec(del);
    QVERIFY(r != 0);
    QCOMPARE(r->hasError(), false);

    QSignalSpy deleteSpy(r, SIGNAL(finished()));
    while (deleteSpy.count() == 0) {
        QTest::qWait(100);
    }
    QCOMPARE(deleteSpy.count(), 1);

    QCOMPARE(r->hasError(), false);
    delete r;

    // Verify that it got deleted
    contactNames.clear();
    r = conn.exec(q);
    QVERIFY(r != 0);
    r->waitForFinished();
    QCOMPARE(r->size(), 3);
    while (r->next()) {
        contactNames[r->binding(0).value().toString()] =
            r->binding(1).value().toString();
    }
    QCOMPARE(contactNames.size(), 3);
    delete r;
}

void tst_QSparqlTrackerDirect::insert_new_urn()
{
    // This test will leave unclean test data in tracker if it crashes.
    QSparqlConnection conn("QTRACKER_DIRECT");
    QSparqlQuery add("insert { ?:addeduri a nco:PersonContact; "
                     "nie:isLogicalPartOf <qsparql-tracker-direct-tests> ;"
                     "nco:nameGiven \"addedname006\" .}",
                     QSparqlQuery::InsertStatement);
    const QSparqlBinding addeduri(conn.createUrn("addeduri"));
    add.bindValue(addeduri);
    QSparqlResult* r = conn.exec(add);
    QVERIFY(r != 0);
    QCOMPARE(r->hasError(), false);
    r->waitForFinished(); // this test is synchronous only
    QCOMPARE(r->hasError(), false);
    delete r;

    // Verify that the insertion succeeded
    QSparqlQuery q("select ?addeduri ?ng {?addeduri a nco:PersonContact; "
                   "nie:isLogicalPartOf <qsparql-tracker-direct-tests> ;"
                   "nco:nameGiven ?ng .}");
    QHash<QString, QSparqlBinding> contactNames;
    r = conn.exec(q);
    QVERIFY(r != 0);
    r->waitForFinished();
    QCOMPARE(r->size(), 4);
    while (r->next()) {
        // qDebug() << r->binding(0).toString() << r->binding(1).toString();
        contactNames[r->binding(1).value().toString()] = r->binding(0);
    }
    QCOMPARE(contactNames.size(), 4);
    QCOMPARE(contactNames["addedname006"].value().toString(),
             addeduri.value().toString());
    delete r;

    // Delete the uri
    QSparqlQuery del("delete { ?:addeduri a rdfs:Resource. }",
                     QSparqlQuery::DeleteStatement);

    del.bindValue(addeduri);
    r = conn.exec(del);
    qDebug() << r->query();
    QVERIFY(r != 0);
    QCOMPARE(r->hasError(), false);
    r->waitForFinished(); // this test is synchronous only
    QCOMPARE(r->hasError(), false);
    delete r;

    // Verify that it got deleted
    contactNames.clear();
    r = conn.exec(q);
    QVERIFY(r != 0);
    r->waitForFinished();
    QCOMPARE(r->size(), 3);
    while (r->next()) {
        contactNames[r->binding(1).value().toString()] = r->binding(0);
    }
    QCOMPARE(contactNames.size(), 3);
    delete r;
}

void tst_QSparqlTrackerDirect::query_with_error()
{
    QSparqlConnection conn("QTRACKER_DIRECT");
    QSparqlQuery q("this is not a valid query");
    QSparqlResult* r = conn.exec(q);
    QVERIFY(r != 0);
    QCOMPARE(r->hasError(), false);
    r->waitForFinished(); // this test is synchronous only
    QCOMPARE(r->hasError(), true);
    QCOMPARE(r->lastError().type(), QSparqlError::StatementError);
    delete r;
}

void tst_QSparqlTrackerDirect::iterate_result()
{
    // This test will print out warnings
    testLogLevel = QtCriticalMsg;
    QSparqlConnection conn("QTRACKER_DIRECT");
    QSparqlQuery q("select ?u ?ng {?u a nco:PersonContact; "
                   "nie:isLogicalPartOf <qsparql-tracker-direct-tests> ;"
                   "nco:nameGiven ?ng .}");
    QSparqlResult* r = conn.exec(q);
    QVERIFY(r != 0);
    QCOMPARE(r->hasError(), false);
    r->waitForFinished(); // this test is syncronous only
    QCOMPARE(r->hasError(), false);
    QCOMPARE(r->size(), 3);

    QVERIFY(r->pos() == QSparql::BeforeFirstRow);
    // This is not a valid position
    for (int i=-1; i <= 2; ++i) {
        QCOMPARE(r->binding(i), QSparqlBinding());
        QVERIFY(r->value(i).isNull());
    }
    QCOMPARE(r->current(), QSparqlResultRow());

    for (int i=0; i<3; ++i) {
        QVERIFY(r->next());
        QCOMPARE(r->pos(), i);

        QVERIFY(r->binding(-1).value().isNull());
        QVERIFY(r->binding(0).value().isNull() == false);
        QVERIFY(r->binding(1).value().isNull() == false);
        QVERIFY(r->binding(2).value().isNull());

        QVERIFY(r->value(-1).isNull());
        QVERIFY(r->value(0).isNull() == false);
        QVERIFY(r->value(1).isNull() == false);
        QVERIFY(r->value(2).isNull());
    }
    QVERIFY(!r->next());
    QVERIFY(r->pos() == QSparql::AfterLastRow);
    // This is not a valid position
    for (int i=-1; i <= 2; ++i) {
        QCOMPARE(r->binding(i), QSparqlBinding());
        QVERIFY(r->value(i).isNull());
    }
    QCOMPARE(r->current(), QSparqlResultRow());

    delete r;
}

void tst_QSparqlTrackerDirect::delete_unfinished_result()
{
    QSparqlConnection conn("QTRACKER_DIRECT");
    QSparqlQuery q("select ?u ?ng {?u a nco:PersonContact; "
                   "nie:isLogicalPartOf <qsparql-tracker-direct-tests> ;"
                   "nco:nameGiven ?ng .}");
    QSparqlResult* r = conn.exec(q);
    QVERIFY(r != 0);
    QCOMPARE(r->hasError(), false);
    delete r;
    // Spin the event loop so that the async callback is called.
    QTest::qWait(1000);
}

void tst_QSparqlTrackerDirect::delete_partially_iterated_result()
{
    QSparqlConnectionOptions opts;
    opts.setOption("dataReadyInterval", 1);

    QSparqlConnection conn("QTRACKER_DIRECT", opts);
    QSparqlQuery q("select ?u ?ng {?u a nco:PersonContact; "
                   "nie:isLogicalPartOf <qsparql-tracker-direct-tests> ;"
                   "nco:nameGiven ?ng .}");
    QSparqlResult* r = conn.exec(q);
    QVERIFY(r != 0);
    QCOMPARE(r->hasError(), false);

    // Wait for some dataReady signals
    QSignalSpy spy(r, SIGNAL(dataReady(int)));
    while (spy.count() < 2)
        QTest::qWait(100);
    delete r;
    // And then spin the event loop so that the async callback is called...
    QTest::qWait(1000);
}

namespace {
class DataReadyListener : public QObject
{
    Q_OBJECT
public:
    DataReadyListener(QSparqlResult* r) : result(r), received(0)
    {
        connect(r, SIGNAL(dataReady(int)),
                SLOT(onDataReady(int)));
    }
public slots:
    void onDataReady(int tc)
    {
        //qDebug() << "Ready" << tc;
        if (result) {
            result->deleteLater();
            result = 0;
        }
        received = tc;
    }
public:
    QSparqlResult* result;
    int received;
};

}

void tst_QSparqlTrackerDirect::delete_nearly_finished_result()
{
    // This is a regression test for a crash bug. Running this shouldn't print
    // out warnings:

    // tst_qsparql_tracker_direct[17909]: GLIB CRITICAL ** GLib-GObject -
    // g_object_unref: assertion `G_IS_OBJECT (object)' failed

    // (It doens't always crash.) Detecting the warnings is a bit of a manual
    // work.

    qDebug() << "delete_nearly_finished_result: no GLIB_CRITICALs should be printed:";

    QSparqlConnectionOptions opts;
    opts.setDataReadyInterval(1);

    QSparqlConnection conn("QTRACKER_DIRECT", opts);
    // A big query returning a lot of results
    QSparqlQuery q("select ?n {?u a nco:PersonContact; nco:nameGiven ?n ;"
                   "nie:isLogicalPartOf <qsparql-tracker-direct-bulk-tests> .}");

    QSparqlResult* r = conn.exec(q);
    QVERIFY(r != 0);
    QCOMPARE(r->hasError(), false);

    DataReadyListener listener(r); // this will delete the result

    // And then spin the event loop. Unfortunately, we don't have anything which
    // we could use for verifying we didn't unref the same object twice (it
    // doesn't always crash).
    QTest::qWait(3000);

}

void tst_QSparqlTrackerDirect::cancel_insert_result()
{
    // This test will leave unclean test data in tracker if it crashes.
    QSparqlConnection conn("QTRACKER_DIRECT");
    QSparqlQuery add("insert { <addeduri001> a nco:PersonContact; "
                     "nie:isLogicalPartOf <qsparql-tracker-direct-tests> ;"
                     "nco:nameGiven \"addedname001\" .}",
                     QSparqlQuery::InsertStatement);

    QSparqlResult* r = conn.exec(add);
    QVERIFY(r != 0);
    QCOMPARE(r->hasError(), false);
    delete r;
    QTest::qWait(3000);

    // Delete the uri
    QSparqlQuery del("delete { <addeduri001> a rdfs:Resource. }",
                     QSparqlQuery::DeleteStatement);

    r = conn.exec(del);
    QVERIFY(r != 0);
    QCOMPARE(r->hasError(), false);
    r->waitForFinished(); // this test is synchronous only
    QCOMPARE(r->hasError(), false);
    delete r;
}

void tst_QSparqlTrackerDirect::result_type_bool()
{
    QSparqlConnection conn("QTRACKER_DIRECT");

    // Boolean result
    QSparqlResult* r = conn.exec(QSparqlQuery("select 1 > 0 { }"));
    QVERIFY(r != 0);
    QCOMPARE(r->hasError(), false);
    r->waitForFinished();
    QVERIFY(r->next());
    QVERIFY(r->value(0).toBool() == true);
    QVERIFY(r->value(0).type() == QVariant::Bool);
    delete r;
    r = conn.exec(QSparqlQuery("select 0 > 1 { }"));
    QVERIFY(r != 0);
    QCOMPARE(r->hasError(), false);
    r->waitForFinished();
    QVERIFY(r->next());
    QVERIFY(r->value(0).toBool() == false);
    QVERIFY(r->value(0).type() == QVariant::Bool);
    delete r;

    // Another type of boolean result
    r = conn.exec(QSparqlQuery("select ?b { <uri004> tracker:available ?b . }"));
    QVERIFY(r != 0);
    QCOMPARE(r->hasError(), false);
    r->waitForFinished();
    QVERIFY(r->next());
    QVERIFY(r->value(0).toBool() == true);
    QVERIFY(r->value(0).type() == QVariant::Bool);
    delete r;

    r = conn.exec(QSparqlQuery("select ?b { <uri005> tracker:available ?b . }"));
    QVERIFY(r != 0);
    QCOMPARE(r->hasError(), false);
    r->waitForFinished();
    QVERIFY(r->next());
    QVERIFY(r->value(0).toBool() == false);
    QVERIFY(r->value(0).type() == QVariant::Bool);
    delete r;
}

void tst_QSparqlTrackerDirect::concurrent_queries()
{
    QSparqlConnection conn("QTRACKER_DIRECT");

    QSparqlQuery q("select ?u ?ng {?u a nco:PersonContact; "
                   "nie:isLogicalPartOf <qsparql-tracker-direct-tests> ;"
                   "nco:nameGiven ?ng .}");
    QSparqlResult* r1 = conn.exec(q);
    QVERIFY(r1 != 0);
    QCOMPARE(r1->hasError(), false);

    QSparqlResult* r2 = conn.exec(q);
    QVERIFY(r2 != 0);
    QCOMPARE(r2->hasError(), false);

    r1->waitForFinished();
    r2->waitForFinished();
    
    QCOMPARE(r1->hasError(), false);
    QCOMPARE(r1->size(), 3);
    delete r1;
    QCOMPARE(r2->hasError(), false);
    QCOMPARE(r2->size(), 3);
    delete r2;
}

void tst_QSparqlTrackerDirect::concurrent_queries_2()
{
    QSparqlConnection conn("QTRACKER_DIRECT");
    QSparqlQuery q("select ?u ?ng {?u a nco:PersonContact; "
                   "nie:isLogicalPartOf <qsparql-tracker-direct-tests> ;"
                   "nco:nameGiven ?ng .}");
    QSparqlResult* r1 = conn.exec(q);
    QVERIFY(r1 != 0);
    QCOMPARE(r1->hasError(), false);

    QSparqlResult* r2 = conn.exec(q);
    QVERIFY(r2 != 0);
    QCOMPARE(r2->hasError(), false);

    while (r1->size() < 3 || r2->size() < 3)
        QTest::qWait(1000);

    QCOMPARE(r1->hasError(), false);
    QCOMPARE(r1->size(), 3);
    delete r1;
    QCOMPARE(r2->hasError(), false);
    QCOMPARE(r2->size(), 3);
    delete r2;
}

void tst_QSparqlTrackerDirect::insert_with_dbus_read_with_direct()
{
    // This test will leave unclean test data in tracker if it crashes.
    QSparqlConnection writeConn("QTRACKER");
    QSparqlConnection readConn("QTRACKER_DIRECT");

    // Insert data with writeconn
    QSparqlQuery add("insert { ?:addeduri a nco:PersonContact; "
                     "nie:isLogicalPartOf <qsparql-tracker-direct-tests> ;"
                     "nco:nameGiven \"addedname006\" .}",
                     QSparqlQuery::InsertStatement);
    const QSparqlBinding addeduri(writeConn.createUrn("addeduri"));
    add.bindValue(addeduri);
    QSparqlResult* r = writeConn.exec(add);
    QVERIFY(r);
    QVERIFY(!r->hasError());
    r->waitForFinished(); // this test is synchronous only
    QVERIFY(!r->hasError());
    delete r;

    // Verify that the insertion succeeded with readConn
    QSparqlQuery q("select ?addeduri ?ng {?addeduri a nco:PersonContact; "
                   "nie:isLogicalPartOf <qsparql-tracker-direct-tests> ;"
                   "nco:nameGiven ?ng .}");
    {
        QHash<QString, QSparqlBinding> contactNames;
        r = readConn.exec(q);
        QVERIFY(r);
        r->waitForFinished();
        QCOMPARE(r->size(), 4);
        while (r->next()) {
            contactNames[r->binding(1).value().toString()] = r->binding(0);
        }
        QCOMPARE(contactNames.size(), 4);
        QCOMPARE(contactNames["addedname006"].value().toString(), addeduri.value().toString());
        delete r;
    }

    // Delete and re-insert data with writeConn
    QSparqlQuery deleteAndAdd("delete { ?:addeduri a rdfs:Resource. } "
                              "insert { ?:addeduri a nco:PersonContact; "
                              "nie:isLogicalPartOf <qsparql-tracker-direct-tests> ;"
                              "nco:nameGiven \"addedname006\" .}",
                              QSparqlQuery::InsertStatement);
    deleteAndAdd.bindValue(addeduri);
    r = writeConn.exec(add);
    QVERIFY(r);
    QVERIFY(!r->hasError());
    r->waitForFinished(); // this test is synchronous only
    QVERIFY(!r->hasError());
    delete r;

    // Verify once more that the insertion succeeded with readConn
    {
        QHash<QString, QSparqlBinding> contactNames;
        r = readConn.exec(q);
        QVERIFY(r);
        r->waitForFinished();
        QCOMPARE(r->size(), 4);
        while (r->next()) {
            // qDebug() << r->binding(0).toString() << r->binding(1).toString();
            contactNames[r->binding(1).value().toString()] = r->binding(0);
        }
        QCOMPARE(contactNames.size(), 4);
        QCOMPARE(contactNames["addedname006"].value().toString(),
                 addeduri.value().toString());
        delete r;
    }

    // Delete the uri
    QSparqlQuery del("delete { ?:addeduri a rdfs:Resource. }",
                     QSparqlQuery::DeleteStatement);

    del.bindValue(addeduri);
    r = writeConn.exec(del);
    qDebug() << r->query();
    QVERIFY(r);
    QVERIFY(!r->hasError());
    r->waitForFinished(); // this test is synchronous only
    QVERIFY(!r->hasError());
    delete r;
}

void tst_QSparqlTrackerDirect::open_connection_twice()
{
    QSparqlQuery q("select ?u ?ng {?u a nco:PersonContact; "
                   "nie:isLogicalPartOf <qsparql-tracker-direct-tests> ;"
                   "nco:nameGiven ?ng .}");
    {
        QSparqlConnection conn("QTRACKER_DIRECT");
        QSparqlResult* r = conn.exec(q);
        QVERIFY(r != 0);
        QCOMPARE(r->hasError(), false);
        r->waitForFinished();
        QCOMPARE(r->size(), 3);
        delete r;
    } // conn goes out of scope

    {
        QSparqlConnection conn("QTRACKER_DIRECT");
        QSparqlResult* r = conn.exec(q);
        QVERIFY(r != 0);
        QCOMPARE(r->hasError(), false);
        r->waitForFinished();
        QCOMPARE(r->size(), 3);
        delete r;
    }
}

void tst_QSparqlTrackerDirect::special_chars()
{
    // This test will leave unclean test data in tracker if it crashes.
    QSparqlConnection conn("QTRACKER_DIRECT");
    QString withSpecialChars("foo\u2780\u2781\u2782");
    QSparqlQuery add(QString("insert { <addeduri002> a nco:PersonContact; "
                             "nie:isLogicalPartOf <qsparql-tracker-direct-tests> ;"
                             "nco:nameGiven \"%1\" .}").arg(withSpecialChars),
                             QSparqlQuery::InsertStatement);

    QSparqlResult* r = conn.exec(add);
    QVERIFY(r != 0);
    QCOMPARE(r->hasError(), false);
    r->waitForFinished(); // this test is synchronous only
    if (r->hasError()) {
        qDebug() << r->lastError().message();
    }
    QCOMPARE(r->hasError(), false);
    delete r;

    // Verify that the insertion succeeded
    QSparqlQuery q("select ?u ?ng {?u a nco:PersonContact; "
                   "nie:isLogicalPartOf <qsparql-tracker-direct-tests> ;"
                   "nco:nameGiven ?ng .}");
    QHash<QString, QString> contactNames;
    r = conn.exec(q);
    QVERIFY(r != 0);
    r->waitForFinished();
    QCOMPARE(r->size(), 4);
    while (r->next()) {
        contactNames[r->binding(0).value().toString()] =
            r->binding(1).value().toString();
    }
    QCOMPARE(contactNames.size(), 4);
    QCOMPARE(contactNames["addeduri002"], withSpecialChars);
    delete r;

    // Delete the uri
    QSparqlQuery del("delete { <addeduri002> a rdfs:Resource. }",
                     QSparqlQuery::DeleteStatement);

    r = conn.exec(del);
    QVERIFY(r != 0);
    QCOMPARE(r->hasError(), false);
    r->waitForFinished(); // this test is synchronous only
    QCOMPARE(r->hasError(), false);
    delete r;
}

void tst_QSparqlTrackerDirect::result_immediately_finished()
{
    QSparqlConnection conn("QTRACKER_DIRECT");
    QSparqlQuery q("select ?u ?ng {?u a nco:PersonContact; "
                   "nie:isLogicalPartOf <qsparql-tracker-direct-tests> ;"
                   "nco:nameGiven ?ng .}");
    QSparqlResult* r = conn.exec(q);
    QVERIFY(r != 0);
    QCOMPARE(r->hasError(), false);

    // No matter how slow this thread is, the result shouldn't get finished
    // behind our back.
    sleep(3);

    QSignalSpy spy(r, SIGNAL(finished()));
    QTime timer;
    timer.start();
    while (spy.count() == 0 && timer.elapsed() < 3000) {
        QTest::qWait(100);
    }

    QCOMPARE(spy.count(), 1);
    delete r;
}

void tst_QSparqlTrackerDirect::result_immediately_finished2()
{
    QSparqlConnection conn("QTRACKER_DIRECT");
    QSparqlQuery q("select ?u ?ng {?u a nco:PersonContact; "
                   "nie:isLogicalPartOf <qsparql-tracker-direct-tests> ;"
                   "nco:nameGiven ?ng .}");
    QSparqlResult* r = conn.exec(q);
    QSignalSpy spy(r, SIGNAL(finished()));

    QVERIFY(r != 0);
    QCOMPARE(r->hasError(), false);

    // No matter how slow this thread is, the result shouldn't get finished
    // behind our back.
    sleep(3);

    // But when we do waitForFinished, the "side effects" like emitting the
    // finished() signal should occur before it returns.
    r->waitForFinished();
    QCOMPARE(spy.count(), 1);

    // And they should not occur again even if we wait here a bit...
    QTest::qWait(1000);
    QCOMPARE(spy.count(), 1);

    delete r;
}

void tst_QSparqlTrackerDirect::delete_connection_immediately()
{
    QSparqlConnection conn("QTRACKER_DIRECT");
}

void tst_QSparqlTrackerDirect::delete_connection_before_a_wait()
{
    {
        QSparqlConnection conn("QTRACKER_DIRECT");
    }
    QTest::qWait(1000);
}

void tst_QSparqlTrackerDirect::go_beyond_columns_number()
{
    QSparqlConnection conn("QTRACKER_DIRECT");
    QSparqlQuery q("select ?u ?ng {?u a nco:PersonContact; "
                   "nie:isLogicalPartOf <qsparql-tracker-direct-tests> ;"
                   "nco:nameGiven ?ng .}");
    QSparqlResult* r = conn.exec(q);
    QVERIFY(r != 0);
    QCOMPARE(r->hasError(), false);
    r->waitForFinished(); // this test is synchronous only
    QCOMPARE(r->hasError(), false);
    QCOMPARE(r->size(), 3);
    while (r->next()) {
        QCOMPARE(r->current().count(), 2);
        QCOMPARE(r->value(5).toString(), QString());
        QCOMPARE(r->binding(5).toString(), QString());
    }
    delete r;
}

void tst_QSparqlTrackerDirect::create_2_connections()
{
    QSparqlConnection conn("QTRACKER_DIRECT");
    QSparqlConnection conn2("QTRACKER_DIRECT"); // this hangs
}

void tst_QSparqlTrackerDirect::unsupported_statement_type()
{
    QSparqlConnection conn("QTRACKER_DIRECT");
    QSparqlQuery q("construct { ?u <http://www.semanticdesktop.org/ontologies/2007/03/22/nco#nameGiven> ?ng } where {?u a nco:PersonContact; "
                   "nie:isLogicalPartOf <qsparql-virtuoso-tests> ;"
                   "nco:nameGiven ?ng .}", QSparqlQuery::ConstructStatement);
    QSparqlResult* r = conn.exec(q);
    QVERIFY(r != 0);
    QCOMPARE(r->hasError(), true);
    delete r;
}

void tst_QSparqlTrackerDirect::async_conn_opening()
{
    QFETCH(int, delayBeforeFirst);
    QFETCH(int, delayBeforeSecond);

    QSparqlConnection conn("QTRACKER_DIRECT");

    QSparqlQuery q("select ?u ?ng {?u a nco:PersonContact; "
                   "nie:isLogicalPartOf <qsparql-tracker-direct-tests> ;"
                   "nco:nameGiven ?ng .}");

    if (delayBeforeFirst > 0)
        QTest::qWait(delayBeforeFirst);

    QSparqlResult* r1 = conn.exec(q);
    QCOMPARE(r1->hasError(), false);
    QSignalSpy spy1(r1, SIGNAL(finished()));

    if (delayBeforeFirst > 0)
        QTest::qWait(delayBeforeSecond);

    QSparqlResult* r2 = conn.exec(q);
    QCOMPARE(r2->hasError(), false);
    QSignalSpy spy2(r2, SIGNAL(finished()));

    // Check that we get the finished() signal
    QTime timer;
    timer.start();
    while ((spy1.count() == 0 || spy2.count() == 0) && timer.elapsed() < 5000) {
        QTest::qWait(1000);
    }
    QCOMPARE(spy1.count(), 1);
    QCOMPARE(spy2.count(), 1);

    // Check that the data is correct
    QHash<QString, QString> contactNames1, contactNames2;
    while (r1->next()) {
        QCOMPARE(r1->current().count(), 2);
        contactNames1[r1->value(0).toString()] = r1->value(1).toString();
    }

    while (r2->next()) {
        QCOMPARE(r2->current().count(), 2);
        contactNames2[r2->value(0).toString()] = r2->value(1).toString();
    }
    QCOMPARE(contactNames1.size(), 3);
    QCOMPARE(contactNames1["uri001"], QString("name001"));
    QCOMPARE(contactNames1["uri002"], QString("name002"));
    QCOMPARE(contactNames1["uri003"], QString("name003"));
    QCOMPARE(contactNames2.size(), 3);
    QCOMPARE(contactNames2["uri001"], QString("name001"));
    QCOMPARE(contactNames2["uri002"], QString("name002"));
    QCOMPARE(contactNames2["uri003"], QString("name003"));

    delete r1;
    delete r2;
}

void tst_QSparqlTrackerDirect::async_conn_opening_data()
{
    QTest::addColumn<int>("delayBeforeFirst");
    QTest::addColumn<int>("delayBeforeSecond");

    QTest::newRow("BothBeforeConnOpened")
        << 0 << 0;

    QTest::newRow("BothAfterConnOpened")
        << 2000 << 0;

    QTest::newRow("BeforeAndAfterConnOpened")
        << 0 << 2000;
}

void tst_QSparqlTrackerDirect::async_conn_opening_with_2_connections()
{
    QFETCH(int, delayBeforeCreatingSecondConnection);

    QSparqlConnection conn1("QTRACKER_DIRECT");

    if (delayBeforeCreatingSecondConnection > 0)
        QTest::qWait(delayBeforeCreatingSecondConnection);

    QSparqlConnection conn2("QTRACKER_DIRECT");

    QSparqlQuery q("select ?u ?ng {?u a nco:PersonContact; "
                   "nie:isLogicalPartOf <qsparql-tracker-direct-tests> ;"
                   "nco:nameGiven ?ng .}");

    QSparqlResult* r1 = conn1.exec(q);
    QCOMPARE(r1->hasError(), false);
    QSignalSpy spy1(r1, SIGNAL(finished()));

    QSparqlResult* r2 = conn2.exec(q);
    QCOMPARE(r2->hasError(), false);
    QSignalSpy spy2(r1, SIGNAL(finished()));

    // Check that we get the finished() signal
    QTime timer;
    timer.start();
    while ((spy1.count() == 0 || spy2.count() == 0) && timer.elapsed() < 5000) {
        QTest::qWait(1000);
    }

    // Check that the data is correct
    QHash<QString, QString> contactNames1, contactNames2;
    while (r1->next()) {
        QCOMPARE(r1->current().count(), 2);
        contactNames1[r1->value(0).toString()] = r1->value(1).toString();
    }

    while (r2->next()) {
        QCOMPARE(r2->current().count(), 2);
        contactNames2[r2->value(0).toString()] = r2->value(1).toString();
    }
    QCOMPARE(contactNames1.size(), 3);
    QCOMPARE(contactNames1["uri001"], QString("name001"));
    QCOMPARE(contactNames1["uri002"], QString("name002"));
    QCOMPARE(contactNames1["uri003"], QString("name003"));
    QCOMPARE(contactNames2.size(), 3);
    QCOMPARE(contactNames2["uri001"], QString("name001"));
    QCOMPARE(contactNames2["uri002"], QString("name002"));
    QCOMPARE(contactNames2["uri003"], QString("name003"));

    delete r1;
    delete r2;
}

void tst_QSparqlTrackerDirect::async_conn_opening_with_2_connections_data()
{
    QTest::addColumn<int>("delayBeforeCreatingSecondConnection");

    QTest::newRow("SecondCreatedBeforeFirstOpened")
        << 0;

    QTest::newRow("SecondCreatedAfterFirstOpened")
        << 2000;
}

void tst_QSparqlTrackerDirect::async_conn_opening_for_update()
{
    QFETCH(int, delayBeforeFirst);
    QFETCH(int, delayBeforeSecond);

    QSparqlConnection conn("QTRACKER_DIRECT");

    QSparqlQuery add1("insert { <addeduri007> a nco:PersonContact; "
                     "nie:isLogicalPartOf <qsparql-tracker-direct-tests> ;"
                     "nco:nameGiven \"addedname007\" .}",
                     QSparqlQuery::InsertStatement);

    QSparqlQuery add2("insert { <addeduri008> a nco:PersonContact; "
                     "nie:isLogicalPartOf <qsparql-tracker-direct-tests> ;"
                     "nco:nameGiven \"addedname008\" .}",
                     QSparqlQuery::InsertStatement);

    if (delayBeforeFirst > 0)
        QTest::qWait(delayBeforeFirst);

    QSparqlResult* r1 = conn.exec(add1);
    QCOMPARE(r1->hasError(), false);
    QSignalSpy spy1(r1, SIGNAL(finished()));

    if (delayBeforeFirst > 0)
        QTest::qWait(delayBeforeSecond);

    QSparqlResult* r2 = conn.exec(add2);
    QCOMPARE(r2->hasError(), false);
    QSignalSpy spy2(r2, SIGNAL(finished()));

    // Check that we get the finished() signal
    QTime timer;
    timer.start();
    while ((spy1.count() == 0 || spy2.count() == 0) && timer.elapsed() < 5000) {
        QTest::qWait(1000);
    }
    QCOMPARE(spy1.count(), 1);
    QCOMPARE(spy2.count(), 1);

    delete r1;
    delete r2;
    
    // Verify that the insertion succeeded
    QSparqlQuery q("select ?u ?ng {?u a nco:PersonContact; "
                   "nie:isLogicalPartOf <qsparql-tracker-direct-tests> ;"
                   "nco:nameGiven ?ng .}");
    QHash<QString, QString> contactNames;
    r1 = conn.exec(q);
    QVERIFY(r1 != 0);
    r1->waitForFinished();
    QCOMPARE(r1->size(), 5);
    while (r1->next()) {
        contactNames[r1->binding(0).value().toString()] =
            r1->binding(1).value().toString();
    }
    QCOMPARE(contactNames.size(), 5);
    QCOMPARE(contactNames["addeduri007"], QString("addedname007"));
    QCOMPARE(contactNames["addeduri008"], QString("addedname008"));
    delete r1;
    
    QSparqlQuery del1("delete { <addeduri007> a rdfs:Resource. }",
                     QSparqlQuery::DeleteStatement);

    r1 = conn.exec(del1);
    QVERIFY(r1 != 0);
    QCOMPARE(r1->hasError(), false);
    r1->waitForFinished(); // this test is synchronous only
    QCOMPARE(r1->hasError(), false);
    delete r1;
    
    QSparqlQuery del2("delete { <addeduri008> a rdfs:Resource. }",
                     QSparqlQuery::DeleteStatement);

    r2 = conn.exec(del2);
    QVERIFY(r2 != 0);
    QCOMPARE(r2->hasError(), false);
    r2->waitForFinished(); // this test is synchronous only
    QCOMPARE(r2->hasError(), false);
    delete r2;
}

void tst_QSparqlTrackerDirect::async_conn_opening_for_update_data()
{
    QTest::addColumn<int>("delayBeforeFirst");
    QTest::addColumn<int>("delayBeforeSecond");

    QTest::newRow("BothBeforeConnOpened")
        << 0 << 0;

    QTest::newRow("BothAfterConnOpened")
        << 2000 << 0;

    QTest::newRow("BeforeAndAfterConnOpened")
        << 0 << 2000;
}

void tst_QSparqlTrackerDirect::dataTypes()
{
    QSparqlConnection conn("QTRACKER_DIRECT");
    QSparqlQuery dataTypes("select "
                           "<qsparql-tracker-direct-tests> "
                           "80 "
                           "23.4 "
                           "true "
                           "false "
                           "\"a string\" "
                           "{ }");
    QSparqlResult* r = conn.exec(dataTypes);
    QVERIFY(r != 0);
    QVERIFY(!r->hasError());
    r->waitForFinished(); // this test is synchronous only

    QVERIFY(r->next());
    QCOMPARE(r->value(0),
             QVariant(QUrl::fromEncoded("qsparql-tracker-direct-tests")));
    QCOMPARE(r->value(1), QVariant(80));
    QCOMPARE(r->value(2), QVariant(23.4));
    QCOMPARE(r->value(3), QVariant(true));
    QCOMPARE(r->value(4), QVariant(false));
    QCOMPARE(r->value(5), QVariant(QString::fromLatin1("a string")));

    // urls don't have data type uris
    QCOMPARE(r->binding(0).dataTypeUri(),
            QUrl::fromEncoded(""));

    QCOMPARE(r->binding(1).dataTypeUri(),
             QUrl::fromEncoded("http://www.w3.org/2001/XMLSchema#integer"));

    QCOMPARE(r->binding(2).dataTypeUri(),
             QUrl::fromEncoded("http://www.w3.org/2001/XMLSchema#double"));

    QCOMPARE(r->binding(3).dataTypeUri(),
             QUrl::fromEncoded("http://www.w3.org/2001/XMLSchema#boolean"));
    QCOMPARE(r->binding(4).dataTypeUri(),
             QUrl::fromEncoded("http://www.w3.org/2001/XMLSchema#boolean"));

    QCOMPARE(r->binding(5).dataTypeUri(),
             QUrl::fromEncoded("http://www.w3.org/2001/XMLSchema#string"));

    delete r;
}

void tst_QSparqlTrackerDirect::explicitDataTypes()
{
    QSparqlConnection conn("QTRACKER_DIRECT");
    QSparqlQuery explicitTypes("select "
                               "<qsparql-tracker-direct-tests> "
                               "\"80\"^^xsd:integer "
                               "\"-7\"^^xsd:int "
                               "\"23.4\"^^xsd:double "
                               "\"true\"^^xsd:boolean "
                               "\"false\"^^xsd:boolean "
                               "\"a string\"^^xsd:string "
                               "\"2011-03-28T09:36:00+02:00\"^^xsd:dateTime "
                               "{ }");
    QSparqlResult* r = conn.exec(explicitTypes);
    QVERIFY(r != 0);
    QVERIFY(!r->hasError());
    r->waitForFinished(); // this test is synchronous only

    QVERIFY(r->next());
    QCOMPARE(r->value(0),
             QVariant(QUrl::fromEncoded("qsparql-tracker-direct-tests")));
    QCOMPARE(r->value(1), QVariant(80));
    QCOMPARE(r->value(2), QVariant(-7));
    QCOMPARE(r->value(3), QVariant(23.4));
    QCOMPARE(r->value(4), QVariant(true));
    QCOMPARE(r->value(5), QVariant(false));
    QCOMPARE(r->value(6), QVariant(QString::fromLatin1("a string")));

    // Tracker seems to return the datetime as a string
    QEXPECT_FAIL("", "Tracker returns dates as strings", Continue);
    QCOMPARE(r->value(7),
             QVariant(QDateTime::fromString("2011-03-28T09:36:00+02:00", Qt::ISODate)));

    // urls don't have data type uris
    QCOMPARE(r->binding(0).dataTypeUri(),
            QUrl::fromEncoded(""));

    QCOMPARE(r->binding(1).dataTypeUri(),
             QUrl::fromEncoded("http://www.w3.org/2001/XMLSchema#integer"));
    QCOMPARE(r->binding(2).dataTypeUri(),
             QUrl::fromEncoded("http://www.w3.org/2001/XMLSchema#integer"));

    QEXPECT_FAIL("", "Tracker returns doubles as strings", Continue);
    QCOMPARE(r->binding(3).dataTypeUri(),
             QUrl::fromEncoded("http://www.w3.org/2001/XMLSchema#double"));

    QCOMPARE(r->binding(4).dataTypeUri(),
             QUrl::fromEncoded("http://www.w3.org/2001/XMLSchema#boolean"));
    QCOMPARE(r->binding(5).dataTypeUri(),
             QUrl::fromEncoded("http://www.w3.org/2001/XMLSchema#boolean"));

    QCOMPARE(r->binding(6).dataTypeUri(),
             QUrl::fromEncoded("http://www.w3.org/2001/XMLSchema#string"));

    QEXPECT_FAIL("", "Tracker returns dates as strings", Continue);
    QCOMPARE(r->binding(7).dataTypeUri(),
             QUrl::fromEncoded("http://www.w3.org/2001/XMLSchema#dateTime"));

    delete r;
}

void tst_QSparqlTrackerDirect::largeInteger()
{
    QSparqlQuery insert("insert {<mydataobject> a nie:DataObject, nie:InformationElement ; "
                        "nie:isLogicalPartOf <qsparql-tracker-live-tests> ;"
                        "nie:byteSize 5000000000 .}", QSparqlQuery::InsertStatement);
    QSparqlConnection conn("QTRACKER_DIRECT");
    QSparqlResult* r = conn.syncExec(insert);
    delete r;
    QSparqlQuery select("select ?do ?bs "
                        "{ ?do a nie:DataObject ; "
                        "nie:isLogicalPartOf <qsparql-tracker-live-tests> ;"
                        "nie:byteSize ?bs .}");
    r = conn.exec(select);
    r->waitForFinished();

    QVERIFY(!r->hasError());
    QVERIFY(r->next());
    QCOMPARE(r->value(1), QVariant(Q_UINT64_C(5000000000)));
    delete r;

    QSparqlQuery clean("delete {<mydataobject> a rdfs:Resource . }",
                       QSparqlQuery::DeleteStatement);
    r = conn.syncExec(clean);
    delete r;
}

namespace {

class DeleteResultOnFinished : public QObject
{
    Q_OBJECT
    public Q_SLOTS:
    void onFinished()
    {
        sender()->deleteLater();
    }
};

}

void tst_QSparqlTrackerDirect::deleteLaterWithSelectResult()
{
    // it doesn't matter what the query is; we don't look at what it returns
    QSparqlQuery query("select ?ie { ?ie a nie:InformationElement . } ");
    QSparqlConnection conn("QTRACKER_DIRECT");
    QSparqlResult* r = conn.exec(query);

    // Start monitoring the destroyed() signal
    QSignalSpy destroyedSpy(r, SIGNAL(destroyed()));

    // When we get the finished() signal of the result, do deleteLater.
    DeleteResultOnFinished resultDeleter;
    connect(r, SIGNAL(finished()), &resultDeleter, SLOT(onFinished()));

    QSignalSpy finishedSpy(r, SIGNAL(finished()));

    // Spin the event loop so that the finished() signal arrives
    QTime timer;
    timer.start();
    while (finishedSpy.count() == 0 && timer.elapsed() < 5000) {
        QTest::qWait(100);
    }
    QCOMPARE(finishedSpy.count(), 1);

    // Now spinning the event loop should make the DeferredDelete event is
    // processed.  (Note: don't do QCoreApplication::sendPostedEvents(0,
    // QEvent::DeferredDelete here, that will make buggy code pass, too.)
    QTest::qWait(100);

    QCOMPARE(destroyedSpy.count(), 1);
}

void tst_QSparqlTrackerDirect::deleteLaterWithUpdateResult()
{
    QSparqlQuery insert("insert {<testresource001> a nie:InformationElement ; "
                        "nie:isLogicalPartOf <qsparql-tracker-live-tests> .}",
                        QSparqlQuery::InsertStatement);
    QSparqlConnection conn("QTRACKER_DIRECT");
    QSparqlResult* r = conn.exec(insert);

    // Start monitoring the destroyed() signal
    QSignalSpy destroyedSpy(r, SIGNAL(destroyed()));

    // When we get the finished() signal of the result, do deleteLater.
    DeleteResultOnFinished resultDeleter;
    connect(r, SIGNAL(finished()), &resultDeleter, SLOT(onFinished()));

    QSignalSpy finishedSpy(r, SIGNAL(finished()));

    // Spin the event loop so that the finished() signal arrives
    QTime timer;
    timer.start();
    while (finishedSpy.count() == 0 && timer.elapsed() < 5000) {
        QTest::qWait(100);
    }
    QCOMPARE(finishedSpy.count(), 1);

    // Now spinning the event loop should make the DeferredDelete event is
    // processed.  (Note: don't do QCoreApplication::sendPostedEvents(0,
    // QEvent::DeferredDelete here, that will make buggy code pass, too.)
    QTest::qWait(100);

    QCOMPARE(destroyedSpy.count(), 1);

    QSparqlQuery clean("delete {<testresource001> a rdfs:Resource . }",
                       QSparqlQuery::DeleteStatement);
    r = conn.syncExec(clean);
    delete r;
}

bool tst_QSparqlTrackerDirect::createTestData(int testDataAmount)
{
    QSparqlConnection conn("QTRACKER_DIRECT");

    const QString insertQuery =
        "INSERT {"
        "<qsparql-tracker-direct-tests> a nie:InformationElement ."
        "<qsparql-tracker-direct-bulk-tests> a nie:InformationElement ."

        "<uri001> a nco:PersonContact, nie:InformationElement ;"
            "nie:isLogicalPartOf <qsparql-tracker-direct-tests> ;"
            "nco:nameGiven \"name001\" ."

        "<uri002> a nco:PersonContact, nie:InformationElement ;"
            "nie:isLogicalPartOf <qsparql-tracker-direct-tests> ;"
            "nco:nameGiven \"name002\" ."

        "<uri003> a nco:PersonContact, nie:InformationElement ;"
            "nie:isLogicalPartOf <qsparql-tracker-direct-tests> ;"
            "nco:nameGiven \"name003\" ."

        "<uri004> a nie:DataObject , nie:InformationElement ;"
            "nie:isLogicalPartOf <qsparql-tracker-direct-tests> ;"
            "tracker:available true ."

        "<uri005> a nie:DataObject , nie:InformationElement ;"
            "nie:isLogicalPartOf <qsparql-tracker-direct-tests> ;"
            "tracker:available false ."
        "}";

    QSparqlResult* r1 = conn.syncExec(QSparqlQuery(insertQuery, QSparqlQuery::InsertStatement));
    if (r1->hasError()) {
        qWarning() << "createTestData() failed:" << r1->lastError() << insertQuery;
        return false;
    }

    const QString insertQueryTemplate =
        "_:u%1 a nco:PersonContact, nie:InformationElement ; "
            "nie:isLogicalPartOf <qsparql-tracker-direct-bulk-tests> ;"
            "nco:nameGiven \"Given%1\" ; "
            "nco:nameFamily \"Family%1\" . ";

    const int insertBatchSize   = 200;

    for (int item = 1; item <= testDataAmount; ) {
        QString insertQuery = "INSERT { ";
        const int batchEnd = item + insertBatchSize;

        for (; item < batchEnd && item <= testDataAmount; ++item) {
            insertQuery.append(insertQueryTemplate.arg(item));
        }

        insertQuery.append(" }");
        QSparqlResult* r2 = conn.syncExec(QSparqlQuery(insertQuery, QSparqlQuery::InsertStatement));
        if (r2->hasError()) {
            qWarning() << "createTestData() failed:" << r2->lastError() << insertQuery;
            return false;
        }
    }

    return true;
}

void tst_QSparqlTrackerDirect::cleanupTestData()
{
    QSparqlConnection conn("QTRACKER_DIRECT");

    const QString deleteQuery1 =
        "DELETE { ?u a rdfs:Resource . } "
        "WHERE { ?u nie:isLogicalPartOf <qsparql-tracker-direct-tests> . }";

    QSparqlResult* r1 = conn.syncExec(QSparqlQuery(deleteQuery1, QSparqlQuery::DeleteStatement));
    if (r1->hasError()) {
        qWarning() << "cleanupTestData() failed:" << r1->lastError() << deleteQuery1;
        return;
    }

    const QString deleteQuery2 =
        "DELETE { <qsparql-tracker-direct-tests> a rdfs:Resource . }";

    QSparqlResult* r2 = conn.syncExec(QSparqlQuery(deleteQuery2, QSparqlQuery::DeleteStatement));
    if (r2->hasError()) {
        qWarning() << "cleanupTestData() failed:" << r2->lastError() << deleteQuery2;
        return;
    }

    const QString deleteQuery3 =
        "DELETE { ?u a rdfs:Resource . } "
        "WHERE { ?u nie:isLogicalPartOf <qsparql-tracker-direct-bulk-tests> . }";

    QSparqlResult* r3 = conn.syncExec(QSparqlQuery(deleteQuery3, QSparqlQuery::DeleteStatement));
    if (r3->hasError()) {
        qWarning() << "cleanupTestData() failed:" << r3->lastError() << deleteQuery3;
        return;
    }

    const QString deleteQuery4 =
        "DELETE { <qsparql-tracker-direct-bulk-tests> a rdfs:Resource . }";

    QSparqlResult* r4 = conn.syncExec(QSparqlQuery(deleteQuery4, QSparqlQuery::DeleteStatement));
    if (r4->hasError()) {
        qWarning() << "cleanupTestData() failed:" << r4->lastError() << deleteQuery4;
        return;
    }

    return;
}

QTEST_MAIN( tst_QSparqlTrackerDirect )
#include "tst_qsparql_tracker_direct.moc"
