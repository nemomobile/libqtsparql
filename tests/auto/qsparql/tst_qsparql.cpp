/****************************************************************************
**
** Copyright (C) 2010-2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (ivan.frade@nokia.com)
**
** This file is part of the test suite of the QtSparql module (not yet part of the Qt Toolkit).
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
** If you have questions regarding the use of this file, please contact
** Nokia at ivan.frade@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtTest/QtTest>
#include <QtSparql/QtSparql>

#include "private/qsparqlconnection_p.h"
#include "private/qsparqldriver_p.h"
#include "../sparqlapitestsutility.h"

class MockDriver;

// class that breaks encapsulation by accessing protected members of QSparqlQueryModel

class tst_Querymodelder : public QSparqlQueryModel
{
    Q_OBJECT

signals:
    void queryChange();

public:
    tst_Querymodelder *testProtected;

public:
    tst_Querymodelder(QObject *parent = 0): QSparqlQueryModel(parent)
    {
    }
    void lastError()
    {
        //nobackend here,so setting the error manulally
        QSparqlConnection connection("QTRACKER");
        QSparqlQuery query("select ?u ?ng ?nf{?u a nco:PersonContact; "
                           "nie:isLogicalPartOf <Virtuoso-DataBase> ;"
                           "nco:nameGiven ?ng;"
                           "nco:nameFamily ?nf.}",QSparqlQuery::AskStatement);
        QSparqlResult *result = connection.exec(query);
        bool check = result->hasError();
        qDebug()<<check<<"check the error";
        QSparqlError error;
        testProtected = new tst_Querymodelder;
        error.setType(QSparqlError::BackendError);
        testProtected->setLastError(error);
    }

    void querychange()
    {
        tst_Querymodelder *testProtected = new tst_Querymodelder;
        QSparqlConnection connection("QTRACKER");
        QString query= "insert { <contact";
        query= query.append("> a nco:PersonContact; "
                            "nie:isLogicalPartOf <Tracker-DataBase> ;"
                            "nco:nameGiven \"personName");
        query= query.append("\"; "
                            "nco:nameFamily \"familyName");
        query= query.append("\".}");
        QSparqlQuery one(query, QSparqlQuery::InsertStatement);
        testProtected->setQuery(one,connection);
        testProtected->querychange();
        QString querytwo= "prefix nco: <http://www.semanticdesktop.org/ontologies/2007/03/22/nco#> "
                "prefix nie: <http://www.semanticdesktop.org/ontologies/2007/01/19/nie#> "
                "insert into <http://virtuoso/testgraph> "
                "{ <contact";
        querytwo= querytwo.append("> a nco:PersonContact; "
                                  "nie:isLogicalPartOf <Virtuoso-DataBase> ;"
                                  "nco:nameGiven \"personName");
        querytwo= querytwo.append("\"; "
                                  "nco:nameFamily \"familyName");
        querytwo= querytwo.append("\".}");
        QSparqlQuery two(querytwo, QSparqlQuery::InsertStatement);
        QSignalSpy spy(testProtected,SIGNAL(queryChange()));
        testProtected->setQuery(two,connection);
        QTestEventLoop::instance().enterLoop(1);

    }

    QModelIndex indexinquery(QModelIndex &item)
    {
        testProtected = new tst_Querymodelder();
        testProtected->indexInQuery(item);
        return testProtected->indexInQuery(item);
    }
};

class MockResult : public QSparqlResult
{
    Q_OBJECT
    public:
    MockResult(const MockDriver* d);
    int size() const
    {
        return size_;
    }

    QSparqlResultRow current() const
    {
        return QSparqlResultRow();
    }

    QSparqlBinding binding(int) const
    {
        return QSparqlBinding();
    }

    QVariant value(int) const
    {
        return QVariant();
    }
public:
    static int size_;
};

class MockSyncFwOnlyResult : public QSparqlResult
{
    Q_OBJECT
    public:
    MockSyncFwOnlyResult()
        : pos(-1) // first row is row 0, we start BeforeFirstRow
    {
    }
    // Only this is needed for iterating the result
    bool next()
    {
        // Do some work to fetch the next row
        if (++pos < size_) { // determine if the row was the last or not
            updatePos(pos);
            return true;
        }
        updatePos(QSparql::AfterLastRow);
        return false;
    }
    bool hasFeature(QSparqlResult::Feature f) const
    {
        if (f == QSparqlResult::ForwardOnly || f == QSparqlResult::Sync)
            return true;
        return false;
    }

    QSparqlResultRow current() const
    {
        return QSparqlResultRow();
    }

    QSparqlBinding binding(int) const
    {
        return QSparqlBinding();
    }

    QVariant value(int) const
    {
        return QVariant();
    }
    int pos;
    static int size_;
};

class MockDriver : public QSparqlDriver
{
    Q_OBJECT
    public:
    MockDriver()
    {
    }
    ~MockDriver()
    {
    }
    bool open(const QSparqlConnectionOptions&)
    {
        ++openCount;
        setOpen(openRetVal);
        return openRetVal;
    }

    void close()
    {
        ++closeCount;
    }
    bool hasFeature(QSparqlConnection::Feature f) const
    {
        if (f == QSparqlConnection::SyncExec || f == QSparqlConnection::AsyncExec)
            return true;
        return false;
    }
    QSparqlResult* exec(const QString&, QSparqlQuery::StatementType, const QSparqlQueryOptions& options)
    {
        switch(options.executionMethod()) {
        case QSparqlQueryOptions::AsyncExec:
            return new MockResult(this);
        case QSparqlQueryOptions::SyncExec:
            return new MockSyncFwOnlyResult();
        default:
            return 0;
        }
    }

    static int openCount;
    static int closeCount;
    static bool openRetVal;
};

int MockResult::size_ = 0;
int MockSyncFwOnlyResult::size_ = 0;

int MockDriver::openCount = 0;
int MockDriver::closeCount = 0;
bool MockDriver::openRetVal = true;

MockResult::MockResult(const MockDriver*)
    : QSparqlResult()
{
}

class MockDriverCreator : public QSparqlDriverCreatorBase
{
    QSparqlDriver* createObject() const
    {
        return new MockDriver();
    }
};

class tst_QSparql : public QObject
{
    Q_OBJECT

public:
    tst_QSparql();
    virtual ~tst_QSparql();

public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:
    void mock_creation();
    void wrong_creation();
    void iterate_error_result();
    void open_fails();
    void connection_scope();
    void drivers_list();

    void iterate_empty_result();
    void iterate_nonempty_result();
    void iterate_nonempty_result_backwards();

    void iterate_empty_fwonly_result();
    void iterate_nonempty_fwonly_result();
    void iterate_nonempty_fwonly_result_first();

    void default_QSparqlQueryOptions();
    void copies_of_QSparqlQueryOptions_are_equal_and_independent();
    void assignment_of_QSparqlQueryOptions_creates_equal_and_independent_copy();
    void qsparqlconnection_test_hasFeature();
    void qsparqlconnection_test_driverName();
    void qsparqlconnection_test_clearPreFixes();
    void qSparqlconnOption_test_cpyConstructor();
    void querymodel_test_HeaderData();
    void querymodel_test_removeColumns();
    void querymodel_test_resultRows();
    void querymodel_test_clear();
    void sparqlError_test_constuctor();
    void sparqlError_test_cpyConstructor();
    void sparqlError_test_operatorassign();
private:
    void removeColumns(QSparqlQuery &  query, QSparqlConnection &  connection);
    void resultRows(QSparqlQuery &  query, QSparqlConnection &  connection);
};

tst_QSparql::tst_QSparql()
{
}

tst_QSparql::~tst_QSparql()
{
}

void tst_QSparql::initTestCase()
{
    qSparqlRegisterConnectionCreator("MOCK", new MockDriverCreator());

    // For running the test without installing the plugins. Should work in
    // normal and vpath builds.
    QCoreApplication::addLibraryPath("../../../plugins");
}

void tst_QSparql::cleanupTestCase()
{
}

void tst_QSparql::init()
{
    MockDriver::openCount = 0;
    MockDriver::closeCount = 0;
    MockDriver::openRetVal = true;
    MockResult::size_ = 0;
    MockSyncFwOnlyResult::size_ = 0;
}

void tst_QSparql::cleanup()
{
}

void tst_QSparql::mock_creation()
{
    QSparqlConnection conn("MOCK");
    QCOMPARE(MockDriver::openCount, 1);
}

void tst_QSparql::wrong_creation()
{
    QSparqlConnection conn("TOTALLYNOTTHERE");
    QSparqlResult* res = conn.exec(QSparqlQuery("foo"));
    QVERIFY(res->hasError());
    QCOMPARE(res->lastError().type(), QSparqlError::ConnectionError);
    delete res;
}

void tst_QSparql::iterate_error_result()
{
    QSparqlConnection conn("TOTALLYNOTTHERE");
    QSparqlResult* res = conn.exec(QSparqlQuery("foo"));
    QVERIFY(res->hasError());
    QVERIFY(!res->next());
    delete res;
}

void tst_QSparql::open_fails()
{
    MockDriver::openRetVal = false;
    QSparqlConnection conn("MOCK");
    QSparqlResult* res = conn.exec(QSparqlQuery("foo"));
    QVERIFY(res->hasError());
    QCOMPARE(res->lastError().type(), QSparqlError::ConnectionError);
}

void tst_QSparql::connection_scope()
{
    {
        QSparqlConnection conn("MOCK");
    }
    QCOMPARE(MockDriver::openCount, 1);
    QCOMPARE(MockDriver::closeCount, 1);
}

void tst_QSparql::drivers_list()
{
    QStringList expectedDrivers;
    expectedDrivers << "QSPARQL_ENDPOINT" << "QTRACKER" << "QTRACKER_DIRECT" << "QVIRTUOSO" << "MOCK";

    QStringList drivers = QSparqlConnection::drivers();
    foreach (const QString& driver, drivers) {
        QVERIFY(expectedDrivers.contains(driver));
        // qDebug() << driver;
    }
    QVERIFY(drivers.size() >= 1);
    QVERIFY(drivers.contains("MOCK"));
}

void tst_QSparql::iterate_empty_result()
{
    QSparqlConnection conn("MOCK");
    QSparqlResult* res = conn.exec(QSparqlQuery("foo"));
    QVERIFY(!res->hasError());
    QVERIFY(res->pos() == QSparql::BeforeFirstRow);
    QVERIFY(!res->next());
    QVERIFY(res->pos() == QSparql::AfterLastRow);
    QVERIFY(!res->previous());
    QVERIFY(res->pos() == QSparql::BeforeFirstRow);
    delete res;
}

void tst_QSparql::iterate_nonempty_result()
{
    QSparqlConnection conn("MOCK");
    QSparqlResult* res = conn.exec(QSparqlQuery("foo"));
    QVERIFY(!res->hasError());
    MockResult::size_ = 2;
    QVERIFY(res->pos() == QSparql::BeforeFirstRow);
    QVERIFY(res->next());
    QCOMPARE(res->pos(), 0);
    QVERIFY(res->next());
    QCOMPARE(res->pos(), 1);
    QVERIFY(!res->next());
    QVERIFY(res->pos() == QSparql::AfterLastRow);
    QVERIFY(!res->next());
    QVERIFY(res->pos() == QSparql::AfterLastRow);
    res->first();
    QVERIFY(res->pos() == 0);
    res->last();
    QVERIFY(res->pos() == 1);
    delete res;
}

void tst_QSparql::iterate_nonempty_result_backwards()
{
    QSparqlConnection conn("MOCK");
    QSparqlResult* res = conn.exec(QSparqlQuery("foo"));
    QVERIFY(!res->hasError());
    MockResult::size_ = 2;
    // Get the result to the last position
    QVERIFY(res->last());
    QVERIFY(!res->next());
    // And then iterate
    QVERIFY(res->pos() == QSparql::AfterLastRow);
    QVERIFY(res->previous());
    QCOMPARE(res->pos(), 1);
    QVERIFY(res->previous());
    QCOMPARE(res->pos(), 0);
    QVERIFY(!res->previous());
    QVERIFY(res->pos() == QSparql::BeforeFirstRow);
    QVERIFY(!res->previous());
    QVERIFY(res->pos() == QSparql::BeforeFirstRow);
    delete res;
}

void tst_QSparql::iterate_empty_fwonly_result()
{
    QSparqlConnection conn("MOCK");
    QSparqlResult* res = conn.syncExec(QSparqlQuery("foo"));
    QVERIFY(!res->hasError());

    MockSyncFwOnlyResult::size_ = 0;
    QVERIFY(res->pos() == QSparql::BeforeFirstRow);
    QVERIFY(!res->next());
    QVERIFY(res->pos() == QSparql::AfterLastRow);
    // previous() always fails, and it doesn't change the position
    QVERIFY(!res->previous());
    QVERIFY(res->pos() == QSparql::AfterLastRow);
    delete res;
}

void tst_QSparql::iterate_nonempty_fwonly_result()
{
    QSparqlConnection conn("MOCK");
    QSparqlResult* res = conn.syncExec(QSparqlQuery("foo"));
    QVERIFY(!res->hasError());
    MockSyncFwOnlyResult::size_ = 2;
    QVERIFY(res->pos() == QSparql::BeforeFirstRow);
    QVERIFY(res->next());
    QCOMPARE(res->pos(), 0);

    // previous() always fails, and it doesn't change the position
    QVERIFY(!res->previous());
    QCOMPARE(res->pos(), 0);

    QVERIFY(res->next());
    QCOMPARE(res->pos(), 1);

    // previous() always fails, and it doesn't change the position
    QVERIFY(!res->previous());
    QCOMPARE(res->pos(), 1);

    // first and last fail, and they don't chane the position
    QVERIFY(!res->first());
    QVERIFY(res->pos() == 1);
    QVERIFY(!res->last());
    QVERIFY(res->pos() == 1);

    // also setPos fails (even if we try setPos(pos() + 1))
    QVERIFY(!res->setPos(0));
    QVERIFY(res->pos() == 1);
    QVERIFY(!res->setPos(2));
    QVERIFY(res->pos() == 1);

    QVERIFY(!res->next());
    QVERIFY(res->pos() == QSparql::AfterLastRow);
    QVERIFY(!res->next());
    QVERIFY(res->pos() == QSparql::AfterLastRow);

    // previous() always fails, and it doesn't change the position
    QVERIFY(!res->previous());
    QVERIFY(res->pos() == QSparql::AfterLastRow);

    delete res;
}

void tst_QSparql::iterate_nonempty_fwonly_result_first()
{
    // A corner case where the user calls first() legally for a forward only
    // result
    QSparqlConnection conn("MOCK");
    QSparqlResult* res = conn.syncExec(QSparqlQuery("foo"));
    QVERIFY(!res->hasError());
    MockSyncFwOnlyResult::size_ = 2;
    QVERIFY(res->pos() == QSparql::BeforeFirstRow);
    // legal first(): just call next() once
    QVERIFY(res->first());
    QCOMPARE(res->pos(), 0);

    // another legal first(), we're at the first row already
    QVERIFY(res->first());
    QCOMPARE(res->pos(), 0);
}

void tst_QSparql::default_QSparqlQueryOptions()
{
    QSparqlQueryOptions opt;
    QCOMPARE( opt.executionMethod(), QSparqlQueryOptions::AsyncExec );
    QCOMPARE( opt.priority(), QSparqlQueryOptions::NormalPriority );
}

void tst_QSparql::copies_of_QSparqlQueryOptions_are_equal_and_independent()
{
    QSparqlQueryOptions opt1;
    opt1.setExecutionMethod(QSparqlQueryOptions::SyncExec);
    QSparqlQueryOptions opt2(opt1);
    QCOMPARE( opt2.executionMethod(), QSparqlQueryOptions::SyncExec );
    QVERIFY( opt2 == opt1 );

    opt2.setPriority(QSparqlQueryOptions::LowPriority);
    QCOMPARE( opt2.priority(), QSparqlQueryOptions::LowPriority );
    QCOMPARE( opt1.priority(), QSparqlQueryOptions::NormalPriority );
    QVERIFY( !(opt2 == opt1) );

    opt2.setPriority(QSparqlQueryOptions::NormalPriority);
    QVERIFY( opt2 == opt1 );
}

void tst_QSparql::assignment_of_QSparqlQueryOptions_creates_equal_and_independent_copy()
{
    QSparqlQueryOptions opt1;
    opt1.setExecutionMethod(QSparqlQueryOptions::SyncExec);
    QSparqlQueryOptions opt2;
    QVERIFY( !(opt2 == opt1) );
    opt2 = opt1;
    QCOMPARE( opt2.executionMethod(), QSparqlQueryOptions::SyncExec );
    QVERIFY( opt2 == opt1 );

    opt2.setPriority(QSparqlQueryOptions::LowPriority);
    QCOMPARE( opt2.priority(), QSparqlQueryOptions::LowPriority );
    QCOMPARE( opt1.priority(), QSparqlQueryOptions::NormalPriority );
    QVERIFY( !(opt2 == opt1) );

    opt2.setPriority(QSparqlQueryOptions::NormalPriority);
    QVERIFY( opt2 == opt1 );
}
void tst_QSparql::qsparqlconnection_test_hasFeature()
{
    QSparqlConnection m_connection("");

    QVERIFY(!m_connection.hasFeature(QSparqlConnection::QuerySize));
    QVERIFY(!m_connection.hasFeature(QSparqlConnection::DefaultGraph));
    QVERIFY(!m_connection.hasFeature(QSparqlConnection::AskQueries));
    QVERIFY(!m_connection.hasFeature(QSparqlConnection::ConstructQueries));
    QVERIFY(!m_connection.hasFeature(QSparqlConnection::UpdateQueries));


    QSparqlConnectionOptions m_options;

    m_options.setHostName("localhost");
    m_options.setDatabaseName("localdatabase");
    m_options.setUserName("dba");
    m_options.setPassword("dba");

    QSparqlConnection m_connection1("QTRACKER",m_options);
    QSparqlQuery m_query("ask {<uri003> a nco:PersonContact; "
                         "nie:isLogicalPartOf <qsparql-tracker-direct-tests> ;"
                         "nco:nameGiven \"name003\" .}", QSparqlQuery::AskStatement);
    QSparqlResult *result = m_connection1.exec(m_query);
    result->waitForFinished();

    QVERIFY(m_connection1.isValid());
    QVERIFY(!m_connection1.isWidgetType());
    QVERIFY(m_connection1.hasFeature((QSparqlConnection::QuerySize)));
    QVERIFY(m_connection1.hasFeature((QSparqlConnection::AskQueries)));
    QVERIFY(m_connection1.hasFeature((QSparqlConnection::UpdateQueries)));
    QVERIFY(!m_connection1.hasFeature((QSparqlConnection::ConstructQueries)));

    delete result;
    result = NULL;
}

void tst_QSparql::qsparqlconnection_test_driverName()
{
    QSparqlConnectionOptions options;

    options.setHostName("localhost");
    options.setDatabaseName("localdatabase");
    options.setUserName("dba");
    options.setPassword("dba");
    options.setPort(8080);
    QSparqlConnection connection("QTRACKER", options);
    QVERIFY(connection.driverName() == QString("QTRACKER"));
}

void tst_QSparql::qsparqlconnection_test_clearPreFixes()
{
    QStringList list=QSparqlConnection::drivers();
    for(int i=0;i<list.count();i++)
    {
        if(list.contains("QSPARQL_ENDPOINT"))  {
            QSparqlConnectionOptions options;
            options.setHostName("localhost");
            options.setPort(8890);
            QSparqlConnection conn("QSPARQL_ENDPOINT", options);
            conn.addPrefix("nco", QUrl("http://www.semanticdesktop.org/ontologies/2007/03/22/nco#"));
            conn.addPrefix("nie", QUrl("http://www.semanticdesktop.org/ontologies/2007/01/19/nie#"));

            QSparqlQuery q("construct { ?u <http://www.semanticdesktop.org/ontologies/2007/03/22/nco#nameGiven> ?ng } where {?u a nco:PersonContact; "
                           "nie:isLogicalPartOf <qsparql-virtuoso-endpoint-tests> ;"
                           "nco:nameGiven ?ng .}", QSparqlQuery::ConstructStatement);
            QSparqlResult* r = conn.exec(q);
            QVERIFY(r != 0);
            QCOMPARE(r->hasError(), false);
            QVERIFY(conn.isValid());
            r->waitForFinished(); // this test is synchronous only
            delete r;
            conn.clearPrefixes();
            QSparqlQuery q2("ask from <http://virtuoso_endpoint/testgraph> { "
                            " ?u a nco:PersonContact; "
                            " nie:isLogicalPartOf <qsparql-virtuoso-endpoint-tests> ; "
                            "nco:nameGiven \"name005\" . }", QSparqlQuery::AskStatement);
            r = conn.exec(q2);
            QVERIFY(r != 0);
            r->waitForFinished(); // this test is synchronous only
            QCOMPARE(r->hasError(), true);
        }
    }
}

void tst_QSparql::qSparqlconnOption_test_cpyConstructor()
{
    QNetworkAccessManager manager1;
    QNetworkAccessManager *manager2,*manager3;
    QNetworkProxy proxy1,proxy2,proxy3;
    QSparqlConnectionOptions option;
    // set the values to the connection options object
    option.setDatabaseName("DRIVER=/usr/lib/odbc/virtodbc_r.so");
    option.setHostName("localhost");
    option.setOption("key",option.hostName());
    option.setPassword("dba");
    option.setPath("sparql");
    option.setPort(8890);
    proxy1.DefaultProxy;
    option.setProxy(proxy1);
    option.setUserName("dba");
    manager1.setProxy(proxy1);
    option.setNetworkAccessManager(&manager1);
    // verify whether its setted or not properly
    QCOMPARE(option.databaseName(),QString("DRIVER=/usr/lib/odbc/virtodbc_r.so"));
    QCOMPARE(option.hostName(), QString("localhost"));
    QCOMPARE(option.option("key"), QVariant(option.hostName()));
    QCOMPARE(option.password(),QString("dba"));
    QCOMPARE(option.path(),QString("sparql"));
    QVERIFY(option.port() ==  8890);
    QCOMPARE(option.userName(),QString("dba"));
    proxy2 = option.proxy();
    manager2 = option.networkAccessManager();
    QVERIFY(manager2->proxy().DefaultProxy == manager2->proxy().DefaultProxy);

    // copy the object to the other object while using the copy constructor
    QSparqlConnectionOptions options1(option);

    // verify both the objects date are same or not
    QCOMPARE(options1.databaseName() , option.databaseName());
    QCOMPARE(options1.hostName(), option.hostName());
    QCOMPARE(options1.option("key"),option.option("key"));
    QCOMPARE(options1.userName() ,option.userName());
    QCOMPARE(options1.password(), option.password());
    QVERIFY(options1.port() == option.port());
    proxy3 = options1.proxy();
    QVERIFY(proxy2.DefaultProxy == proxy3.DefaultProxy);
    manager3 = options1.networkAccessManager();
    QVERIFY(manager3->proxy().DefaultProxy == manager2->proxy().DefaultProxy);
}

void tst_QSparql::querymodel_test_HeaderData()
{
    QSparqlQueryModel querymodel;
    tst_Querymodelder der;

    QSignalSpy modelSpy(&querymodel, SIGNAL(finished()));
    QSignalSpy derSpy(&der, SIGNAL(finished()));

    SparqlApiTestsUtilities util;
    QSparqlResult *r;
    QModelIndex modelIndex;

    QSparqlConnection conn("QTRACKER");
    bool createDb = util.createSampleDataBase(QString("QTRACKER"));
    QVERIFY(createDb);
    QSparqlQuery query("select ?u ?ng ?nf{?u a nco:PersonContact; "
                       "nie:isLogicalPartOf <Tracker-DataBase> ;"
                       "nco:nameGiven ?ng;"
                       "nco:nameFamily ?nf.}");
    r = conn.exec(query);
    r->waitForFinished(); // this test is syncronous only
    //querymodel.fetchMore(); // nec

    query.setType(QSparqlQuery::SelectStatement);
    querymodel.setQuery(query,conn);

    if(r->size() > 0) {
        // set the the specified role to value
        querymodel.insertColumns(0, 2,modelIndex);
        // Returns true if orientation is Qt::Horizontal and the section  refers to a valid section
        QVERIFY(querymodel.setHeaderData(0, Qt::Horizontal,"Header"));
        QCOMPARE(querymodel.headerData(0,Qt::Horizontal).toString(),QString("Header"));
    }

    // testing with derived class object
    der.setQuery(query,conn);
    der.insertColumns(0, 2,modelIndex);
    QVERIFY(der.setHeaderData(0, Qt::Horizontal,"Header"));
    QCOMPARE(der.headerData(0,Qt::Horizontal).toString(),QString("Header"));

    // delete the data base
    bool deleteDb = util.deleteSampleDataBase(QString("QTRACKER"));
    QVERIFY(deleteDb);
    delete r;

    // wait for query models to finish their results before they
    // get deleted, avoids warnings
    QTime timer;
    timer.start();
    while ( (modelSpy.count() == 0 || derSpy.count() == 0) && timer.elapsed() < 3000) {
        QTest::qWait(100);
    }
}

// tests the removeColumn to the current query.
void tst_QSparql::querymodel_test_removeColumns()
{
    SparqlApiTestsUtilities util;
    QStringList backendDriverList;

    backendDriverList  = util.availablebackends(); //get the list of available backends
    for (int i=0; i<backendDriverList.count();i++) {
        if (backendDriverList.at(i) == QString("QVIRTUOSO")) {
            QSparqlConnectionOptions options;
            options.setHostName("localhost");
            options.setDatabaseName("DRIVER=/usr/lib/odbc/virtodbc_r.so");
            options.setPort(8890);
            QSparqlConnection connection(backendDriverList.at(i),options);
            QVERIFY(connection.isValid());
            bool createDb = util.createSampleDataBase(QString("QVIRTUOSO"));
            QVERIFY(createDb);

            QSparqlQuery query("prefix nco: <http://www.semanticdesktop.org/ontologies/2007/03/22/nco#> "
                               "prefix nie: <http://www.semanticdesktop.org/ontologies/2007/01/19/nie#> "
                               "select ?u ?ng "
                               "from <http://virtuoso_endpoint/testgraph> "
                               " {?u a nco:PersonContact; "
                               "nie:isLogicalPartOf <qsparql-virtuoso-endpoint-tests> ;"
                               "nco:nameGiven ?ng .}");

            removeColumns(query,connection);
            bool deleteDb = util.deleteSampleDataBase(QString("QVIRTUOSO"));
            QVERIFY(deleteDb);
        } else if (backendDriverList.at(i) == QString("QTRACKER")) {
            QSparqlConnection connection(backendDriverList.at(i));
            QVERIFY(connection.isValid());

            bool createDb = util.createSampleDataBase(QString("QTRACKER"));
            QVERIFY(createDb);
            QSparqlQuery query("select ?u ?ng ?nf{?u a nco:PersonContact; "
                               "nie:isLogicalPartOf <Tracker-DataBase> ;"
                               "nco:nameGiven ?ng;"
                               "nco:nameFamily ?nf.}");
            removeColumns(query,connection);

            // Delete the database.
            bool deleteDb = util.deleteSampleDataBase(QString("QTRACKER"));
            QVERIFY(deleteDb);
        } else if (backendDriverList.at(i) == QString("QTRACKER_DIRECT")) {
            QSparqlConnection connection(backendDriverList.at(i));
            QVERIFY(connection.isValid());

            bool createDb = util.createSampleDataBase(QString("QTRACKER_DIRECT"));
            QVERIFY(createDb);
            QSparqlQuery query("select ?u ?ng ?nf{?u a nco:PersonContact; "
                               "nie:isLogicalPartOf <Tracker-DataBase> ;"
                               "nco:nameGiven ?ng;"
                               "nco:nameFamily ?nf.}");
            removeColumns(query,connection);

            // Delete the database.
            bool deleteDb = util.deleteSampleDataBase(QString("QTRACKER_DIRECT"));
            QVERIFY(deleteDb);
        }
    }
}

//  tests the resultRows for the row passed as argumnet for the current query.
void tst_QSparql::querymodel_test_resultRows()
{
    SparqlApiTestsUtilities util;
    QStringList backendDriverList;

    backendDriverList  = util.availablebackends();   //get the list of available backends

    for (int i=0; i<backendDriverList.count();i++) {
        if(backendDriverList.at(i) == QString("QVIRTUOSO")) {
            QSparqlConnectionOptions options;
            options.setHostName("localhost");
            options.setDatabaseName("DRIVER=/usr/lib/odbc/virtodbc_r.so");
            options.setPort(8890);
            QSparqlConnection connection(backendDriverList.at(i),options);
            QVERIFY(connection.isValid());
            bool createDb = util.createSampleDataBase(QString("QVIRTUOSO"));
            QVERIFY(createDb);

            QSparqlQuery query("prefix nco: <http://www.semanticdesktop.org/ontologies/2007/03/22/nco#> "
                               "prefix nie: <http://www.semanticdesktop.org/ontologies/2007/01/19/nie#> "
                               "select ?u ?ng "
                               "from <http://virtuoso_endpoint/testgraph> "
                               " {?u a nco:PersonContact; "
                               "nie:isLogicalPartOf <qsparql-virtuoso-endpoint-tests> ;"
                               "nco:nameGiven ?ng .}");
            resultRows(query,connection);
            bool deleteDb = util.deleteSampleDataBase(QString("QVIRTUOSO"));
            QVERIFY(deleteDb);
        } else if (backendDriverList.at(i) == QString("QSPARQL_ENDPOINT")) {
            QSparqlConnectionOptions options;
            options.setHostName("dbpedia.org");
            options.setPort(8890);
            QSparqlConnection connection(backendDriverList.at(i),options);
            QVERIFY(connection.isValid());

            /*QSparqlQuery query("SELECT DISTINCT ?Object ?PlaceOfBirth "
                                     "WHERE { "
                                     "<http://dbpedia.org/resource/The_Beatles> <http://dbpedia.org/property/currentMembers> ?Object . "
                                     "?Object <http://dbpedia.org/ontology/birthPlace> ?PlaceOfBirth . }");
                */
            QSparqlQuery query("SELECT DISTINCT ?Object ?PlaceOfBirth "
                               "WHERE { "
                               "<http://dbpedia.org/resource/The_Beatles> <http://dbpedia.org/property/currentMembers> ?Object . "
                               "?Object <http://dbpedia.org/ontology/birthPlace> ?PlaceOfBirth . }");
            resultRows(query, connection);
        } else if (backendDriverList.at(i) == QString("QTRACKER")) {
            QSparqlConnection connection(backendDriverList.at(i));
            QVERIFY(connection.isValid());

            bool createDb = util.createSampleDataBase(QString("QTRACKER"));
            QVERIFY(createDb);
            QSparqlQuery query("select ?u ?ng ?nf{?u a nco:PersonContact; "
                               "nie:isLogicalPartOf <Tracker-DataBase> ;"
                               "nco:nameGiven ?ng;"
                               "nco:nameFamily ?nf.}");
            resultRows(query,connection);
            bool deleteDb = util.deleteSampleDataBase(QString("QTRACKER"));
            QVERIFY(deleteDb);
        } else if (backendDriverList.at(i) == QString("QTRACKER_DIRECT")) {
            QSparqlConnection connection(backendDriverList.at(i));
            QVERIFY(connection.isValid());

            bool createDb = util.createSampleDataBase(QString("QTRACKER_DIRECT"));
            QVERIFY(createDb);
            QSparqlQuery query("select ?u ?ng ?nf{?u a nco:PersonContact; "
                               "nie:isLogicalPartOf <Tracker-DataBase> ;"
                               "nco:nameGiven ?ng;"
                               "nco:nameFamily ?nf.}");
            resultRows(query,connection);
            bool deleteDb = util.deleteSampleDataBase(QString("QTRACKER_DIRECT"));
            QVERIFY(deleteDb);
        }
    }
}

// helper function for removing the column.
void tst_QSparql::removeColumns(QSparqlQuery &  query,QSparqlConnection &  connection)
{
    QSparqlQueryModel queryModel;
    QSparqlResult* result = NULL;
    QModelIndex modelIndex;

    query.setType(QSparqlQuery::SelectStatement);
    queryModel.setQuery(query, connection);
    result = connection.exec(query);
    QVERIFY(result != 0);
    QCOMPARE(result->hasError(), false);

    // Here we don't wait for the query to finninsh. Test the columnCount of the cached database.
    if (result->size()) {
        QVERIFY(queryModel.columnCount(modelIndex) > 0);
    }

    // wait for the query to be finnished.
    result->waitForFinished(); // this test is syncronous only
    QTest::qWait(1000);

    int totalCol = queryModel.columnCount(modelIndex);

    // remove the column out of bound.
    QVERIFY( (queryModel.removeColumns(totalCol+ 2, 2,modelIndex)) == false);

    // removeColumns out of bound.
    QVERIFY( (queryModel.removeColumns(-1, 2,modelIndex)) == false);

    // removeColumns inbound.
    if (totalCol) {
        QVERIFY( (queryModel.removeColumns(0,1,modelIndex)) == true);
    }
}

// helper function for getting the result rows.
void tst_QSparql::resultRows(QSparqlQuery &  query,QSparqlConnection &  connection)
{
    QSparqlQueryModel queryModel;
    QSparqlResult* result = NULL;

    QModelIndex modelIndex;

    query.setType(QSparqlQuery::SelectStatement);
    queryModel.setQuery(query, connection);
    result = connection.exec(query);
    QVERIFY(result != 0);
    QCOMPARE(result->hasError(), false);

    //wait for the query to be finnished.
    result->waitForFinished(); // this test is syncronous only

    QTest::qWait(1000);

    QSparqlResultRow resultRow = queryModel.resultRow();
    // Verify the result row.
    if (result->size()) {
        QVERIFY(resultRow.count());
    }
}

void tst_QSparql::querymodel_test_clear()
{
    QSparqlQueryModel querymodel;
    SparqlApiTestsUtilities util;
    QSparqlResult *result;
    QModelIndex modelIndex;

    QSparqlConnection connection("QTRACKER");
    QVERIFY(connection.isValid());

    bool createDb = util.createSampleDataBase(QString("QTRACKER"));
    QVERIFY(createDb);
    QSparqlQuery query("select ?u ?ng ?nf{?u a nco:PersonContact; "
                       "nie:isLogicalPartOf <Tracker-DataBase> ;"
                       "nco:nameGiven ?ng;"
                       "nco:nameFamily ?nf.}");

    querymodel.setQuery(query, connection);
    result = connection.exec(query);
    QVERIFY(result!=0);
    QVERIFY(result->lastError().type() == QSparqlError::NoError);
    /* insert the columns */
    querymodel.insertColumns(0, 2,modelIndex);
    QVERIFY(querymodel.columnCount(modelIndex) == 2);

    /* clear the resource */
    querymodel.clear();

    /* the model is not usable until the setQuery()  has been called. */
    QVERIFY(querymodel.columnCount(modelIndex) <= 0);
    util.deleteSampleDataBase("QTRACKER");
    delete result;
}

void tst_QSparql::sparqlError_test_constuctor()
{
    QString message = "error";
    int number = -1;

    /* pass the error message,type and number to the construcotr  */
    QSparqlError error(message,QSparqlError::NoError,number);

    /* verify whether values are set or not */
    QVERIFY(error.type() == error.NoError );
    QCOMPARE(error.message(),message);
    QVERIFY(error.number() == number);
}

void tst_QSparql::sparqlError_test_cpyConstructor()
{
    QSparqlError error1;

    /* Set the values to the object */
    error1.setMessage("syntaxerror");
    error1.setNumber(-1);
    error1.setType(QSparqlError::UnknownError);

    QCOMPARE(error1.message(),QString("syntaxerror"));
    QVERIFY(error1.number() == -1);
    QVERIFY(error1.type() == QSparqlError::UnknownError);

    /* copy one object to other */
    QSparqlError error2(error1);

    /* Check the details of the object */
    QCOMPARE(error2.message() , error1.message());
    QVERIFY(error2.number() == error1.number());
    QVERIFY(error2.type() == error1.type());
}

void tst_QSparql::sparqlError_test_operatorassign()
{
    QSparqlError error1,error2;
    error1.setMessage("TransactionError");
    error1.setNumber(3);
    error1.setType(QSparqlError::TransactionError);
    QCOMPARE(error1.message(),QString("TransactionError"));
    QVERIFY(error1.number() == 3);
    QVERIFY(error1.type() == QSparqlError::TransactionError);

    /* assign on object to other */
    error2 = error1;

    QCOMPARE(error2.message() , error1.message());
    QVERIFY(error2.number() == error1.number());
    QVERIFY(error2.type() == error1.type());
}

QTEST_MAIN(tst_QSparql)
#include "tst_qsparql.moc"

