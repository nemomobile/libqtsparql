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

#include <QUrl>

class tst_QSparqlQuery : public QObject
{
    Q_OBJECT

public:
    tst_QSparqlQuery();
    virtual ~tst_QSparqlQuery();

public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:
    void replacement_data();
    void replacement();
    void unbind_and_replace_data();
    void unbind_and_replace();
    void different_datatypes_data();
    void different_datatypes();
    void copyConstructor();
    void assignmentOperator();
    void setQuery();
    void setType();
    void bindValues();
};

tst_QSparqlQuery::tst_QSparqlQuery()
{
}

tst_QSparqlQuery::~tst_QSparqlQuery()
{
}

void tst_QSparqlQuery::initTestCase()
{
    // For running the test without installing the plugins. Should work in
    // normal and vpath builds.
    QCoreApplication::addLibraryPath("../../../plugins");
}

void tst_QSparqlQuery::cleanupTestCase()
{
}

void tst_QSparqlQuery::init()
{
}

void tst_QSparqlQuery::cleanup()
{
}

void tst_QSparqlQuery::replacement_data()
{
    QTest::addColumn<QString>("rawString");
    QTest::addColumn<QStringList>("placeholders");
    QTest::addColumn<QVariantList>("replacements");
    QTest::addColumn<QString>("replacedString");

    QTest::newRow("nothing") <<
        QString("nothing to replace") <<
        (QStringList() << "foo" << "bar") <<
        (QVariantList() << "FOO" << "BAR") <<
        QString("nothing to replace");

    QTest::newRow("simple") <<
        QString("replace ?:foo ?:bar") <<
        (QStringList() << "foo" << "bar") <<
        (QVariantList() << "FOO" << "BAR") <<
        QString("replace \"FOO\" \"BAR\"");

    QTest::newRow("length_changes") <<
        QString("replace ?:foo and ?:bar also") <<
        (QStringList() << "foo" << "bar") <<
        (QVariantList() << "FOOVAL" << "BARVAL") <<
        QString("replace \"FOOVAL\" and \"BARVAL\" also");

    QTest::newRow("quoted") <<
        QString("do not replace '?:foo' or '?:bar'") <<
        (QStringList() << "foo" << "bar") <<
        (QVariantList() << "FOOVAL" << "BARVAL") <<
        QString("do not replace '?:foo' or '?:bar'");

    QTest::newRow("reallife") <<
        QString("insert { _:c a nco:Contact ; "
                "nco:fullname ?:username ; "
                "nco:hasPhoneNumber _:pn . "
                "_:pn a nco:PhoneNumber ; "
                "nco:phoneNumber ?:userphone . }") <<
        (QStringList() << "username" << "userphone") <<
        (QVariantList() << "NAME" << "PHONE") <<
        QString("insert { _:c a nco:Contact ; "
                "nco:fullname \"NAME\" ; "
                "nco:hasPhoneNumber _:pn . "
                "_:pn a nco:PhoneNumber ; "
                "nco:phoneNumber \"PHONE\" . }");

    QTest::newRow("escape_quotes") <<
        QString("the ?:value goes here") <<
        (QStringList() << "value") <<
        (QVariantList() << "some\"thing") <<
        QString("the \"some\\\"thing\" goes here");
}

void tst_QSparqlQuery::replacement()
{
    QFETCH(QString, rawString);
    QFETCH(QStringList, placeholders);
    QFETCH(QVariantList, replacements);
    QFETCH(QString, replacedString);
    QSparqlQuery q(rawString);

    for (int i = 0; i < placeholders.size(); ++i)
        q.bindValue(placeholders[i], replacements[i]);

    QCOMPARE(q.query(), rawString);
    QCOMPARE(q.preparedQueryText(), replacedString);

    // convenience accessor for bound values
    QMap<QString, QSparqlBinding> bv = q.boundValues();

    for (int i = 0; i < placeholders.size(); ++i) {
        QCOMPARE(q.boundValue(placeholders[i]), replacements[i]);
        QVERIFY(bv.contains(placeholders[i]));
        QCOMPARE(bv[placeholders[i]].value(), replacements[i]);
    }
    QCOMPARE(bv.size(), placeholders.size());
}

void tst_QSparqlQuery::unbind_and_replace_data()
{
    return replacement_data();
}

void tst_QSparqlQuery::unbind_and_replace()
{
    QFETCH(QString, rawString);
    QFETCH(QStringList, placeholders);
    QFETCH(QVariantList, replacements);
    QFETCH(QString, replacedString);
    QSparqlQuery q(rawString, QSparqlQuery::InsertStatement);
    q.unbindValues();

    for (int i = 0; i < placeholders.size(); ++i)
        q.bindValue(placeholders[i], replacements[i]);

    QCOMPARE(q.query(), rawString);
    QCOMPARE(q.preparedQueryText(), replacedString);
}

void tst_QSparqlQuery::different_datatypes_data()
{
    QTest::addColumn<QString>("rawString");
    QTest::addColumn<QStringList>("placeholders");
    QTest::addColumn<QVariantList>("replacements");
    QTest::addColumn<QString>("replacedString");

    QTest::newRow("int") <<
        QString("the ?:value goes here") <<
        (QStringList() << "value") <<
        (QVariantList() << QVariant(64)) <<
        QString("the 64 goes here");

    QTest::newRow("int") <<
        QString("the $:value goes here") <<
        (QStringList() << "value") <<
        (QVariantList() << QVariant(64)) <<
        QString("the 64 goes here");

    /*QTest::newRow("double") <<
        QString("the ?:value goes here") <<
        (QStringList() << "?:value") <<
        (QVariantList() << -3.1) <<
        QString("the -3.1 goes here");*/

    QTest::newRow("bool") <<
        QString("the ?:value goes here") <<
        (QStringList() << "value") <<
        (QVariantList() << true) <<
        QString("the true goes here");

    QTest::newRow("string") <<
        QString("the ?:value goes here") <<
        (QStringList() << "value") <<
        (QVariantList() << "cat") <<
        QString("the \"cat\" goes here");

    QTest::newRow("url") <<
        QString("the ?:value goes here") <<
        (QStringList() << "value") <<
        (QVariantList() << QUrl("http://www.test.com")) <<
        QString("the <http://www.test.com> goes here");
}

void tst_QSparqlQuery::different_datatypes()
{
    QFETCH(QString, rawString);
    QFETCH(QStringList, placeholders);
    QFETCH(QVariantList, replacements);
    QFETCH(QString, replacedString);
    QSparqlQuery q(rawString, QSparqlQuery::InsertStatement);

    for (int i = 0; i < placeholders.size(); ++i)
        q.bindValue(placeholders[i], replacements[i]);

    QCOMPARE(q.query(), rawString);
    QCOMPARE(q.preparedQueryText(), replacedString);
}

void tst_QSparqlQuery::setQuery()
{
    QSparqlQuery q("This is sample query");
    QCOMPARE(q.query().isEmpty(), false);
    QCOMPARE(q.query(), QString("This is sample query"));

    q.setQuery("This is changed query");
    QCOMPARE(q.query().isEmpty(), false);
    QCOMPARE(q.query(), QString("This is changed query"));
}

//QSparqlQuery SetType() API property test which sets the type for query
void tst_QSparqlQuery::setType()
{
    QStringList list = QSparqlConnection::drivers();
    for(int i=0;i<list.count();i++)
    {
        if (list.contains("QTRACKER")) {
            //Creating the QTRACKER Database
            QSparqlConnection conn("QTRACKER");
            QVERIFY(conn.isValid());

            // use InsertStatement to insert query into QTRACKER
            QSparqlQuery query("insert { <abcdef> a nco:PersonContact; "
                               "nie:isLogicalPartOf <QTRACKER-database> ;"
                               "nco:nameGiven \"3435346\" .}");
            QVERIFY(query.type() != QSparqlQuery::InsertStatement);
            query.setType(QSparqlQuery::InsertStatement);
            QSparqlResult *result = conn.exec(query);
            QVERIFY(result != 0);
            QVERIFY(result->hasError() == false);
            result->waitForFinished();
            QVERIFY(result->isFinished());
            QVERIFY(result->hasError() ==  false);
            QVERIFY(result->lastError().type() == QSparqlError::NoError);
            QVERIFY(query.type() == QSparqlQuery::InsertStatement);

            // use SelectStatement to select query from QTRACKER
            query.setQuery("select ?u ?ng {?u a nco:PersonContact; "
                           "nie:isLogicalPartOf <QTRACKER-database> ;"
                           "nco:nameGiven ?ng .}");

            QVERIFY(query.type() != QSparqlQuery::SelectStatement);
            query.setType(QSparqlQuery::SelectStatement);
            result = conn.exec(query);
            QVERIFY(result != 0);
            QVERIFY(result->hasError() == false);
            result->waitForFinished();
            QVERIFY(result->isFinished());
            QVERIFY(result->hasError() ==  false);
            QVERIFY(result->lastError().type() == QSparqlError::NoError);
            QVERIFY(query.type() == QSparqlQuery::SelectStatement);

            // use DeleteStatement to delete query from QTRACKER
            query.setQuery("delete{<abcdef> a rdfs:Resource. }");
            QVERIFY(query.type() != QSparqlQuery::DeleteStatement);
            query.setType(QSparqlQuery::DeleteStatement);
            result = conn.exec(query);
            QVERIFY(result != 0);
            QVERIFY(result->hasError() == false);
            result->waitForFinished();
            QVERIFY(result->isFinished());
            QVERIFY(result->hasError() ==  false);
            QVERIFY(result->lastError().type() == QSparqlError::NoError);
            QVERIFY(query.type() == QSparqlQuery::DeleteStatement);
        }
        if (list.contains("QSPARQL_ENDPOINT")) {
            //Creating the EndPoint Database
            QSparqlConnectionOptions options;
            options.setHostName("dbpedia.org");
            QSparqlConnection conn1("QSPARQL_ENDPOINT", options);
            QCOMPARE(conn1.isValid(), true);
            QSparqlResult *result;
            // Use ConstructStatement to construct the query from ENDPOINT
            QSparqlQuery query1("CONSTRUCT { <http://dbpedia.org/resource/The_Beatles> <http://dbpedia.org/property/currentMembers> ?Object } "
                                "WHERE { <http://dbpedia.org/resource/The_Beatles> <http://dbpedia.org/property/currentMembers> ?Object . }");
            QVERIFY(query1.type() != QSparqlQuery::ConstructStatement);
            query1.setType(QSparqlQuery::ConstructStatement);
            result = conn1.exec(query1);
            QVERIFY(result != 0);
            QVERIFY(result->hasError() == false);
            result->waitForFinished();
            QVERIFY(result->isFinished());
            QVERIFY(result->hasError() ==  false);
            QVERIFY(result->lastError().type() == QSparqlError::NoError);
            QVERIFY(query1.type() == QSparqlQuery::ConstructStatement);

            // Use DescribeStatement to Describe the query from ENDPOINT
            query1.setQuery("DESCRIBE ?Object "
                            "WHERE { <http://dbpedia.org/resource/The_Beatles> <http://dbpedia.org/property/currentMembers> ?Object . }");
            QVERIFY(query1.type() != QSparqlQuery::DescribeStatement);
            query1.setType(QSparqlQuery::DescribeStatement);
            result = conn1.exec(query1);
            QVERIFY(result != 0);
            QVERIFY(result->hasError() == false);
            result->waitForFinished();
            QVERIFY(result->isFinished());
            QVERIFY(result->hasError() ==  false);
            QVERIFY(result->lastError().type() == QSparqlError::NoError);
            QVERIFY(query1.type() == QSparqlQuery::DescribeStatement);

            // Use AskStatement to ask the query from ENDPOINT
            query1.setQuery("ASK { <http://dbpedia.org/resource/The_Beatles> <http://dbpedia.org/property/currentMembers> <http://dbpedia.org/resource/Ringo_Starr> . }");
            QVERIFY(query1.type() != QSparqlQuery::AskStatement);
            query1.setType(QSparqlQuery::AskStatement);
            result = conn1.exec(query1);
            QVERIFY(result != 0);
            QVERIFY(result->hasError() == false);
            result->waitForFinished();
            QVERIFY(result->isFinished());
            QVERIFY(result->hasError() ==  false);
            QVERIFY(result->lastError().type() == QSparqlError::NoError);
            QVERIFY(query1.type() == QSparqlQuery::AskStatement);
            delete result;
        }
    }
}

//QSparqlQuery Opearator=() API property test which assign one QSparqlQuery Object to other by using Operator =
void tst_QSparqlQuery::assignmentOperator()
{
    QSparqlConnection conn("QTRACKER");
    QVERIFY(conn.isValid());

    // check whether a valid query is inserted
    QSparqlQuery query("insert { <sdsad> a nco:PersonContact; "
                       "nie:isLogicalPartOf <QTRACKER-database> ;"
                       "nco:nameGiven \"343534\" .}",
                       QSparqlQuery::InsertStatement);
    QSparqlQuery Copyquery = QSparqlQuery(query.query(),query.type());

    QSparqlResult *result = conn.exec(Copyquery);
    QVERIFY(result != 0);
    QVERIFY(result->hasError() == false);
    result->waitForFinished();
    QVERIFY(result->isFinished());
    QVERIFY(result->hasError() ==  false);
    QVERIFY(result->lastError().type() == QSparqlError::NoError);
    //Comparing both the queries
    QCOMPARE(Copyquery .query() ,query.query());
    QCOMPARE(Copyquery.type(), query.type());
    delete result;
}

// bindValues() API property test which Iterates through the variable name - value
// pairs from the bindings  and adds them as bindings.
void tst_QSparqlQuery::bindValues()
{
    //construct an instance of QSparqlBinding class by passing name and value as an argument
    QSparqlBinding bind0("name0", "value0");
    QSparqlBinding bind1("name1");
    QSparqlBinding bind2;
    bind2.setName("name2");
    QSparqlBinding bind3;
    bind3.setValue("value3");

    //Create an instance of QSparqlResultRow class and append the created binding.
    QSparqlResultRow row;
    row.append(bind0);
    row.append(bind1);
    row.append(bind2);

    //Create an instance of QSparqlQuery class and Iterate through the variable name-value
    //pairs from the bindings and add them as bindings. 
    QSparqlQuery q;
    q.bindValues(row);

    //Return a map of the bound values using QSparqlQuery::boundValues and
    //verify it contains the binded name value.
    QMap<QString,QSparqlBinding> map = q.boundValues();
    QVERIFY(map.contains("name0"));
    QVERIFY(map.contains("name1"));
    QVERIFY(map.contains("name2"));
    QVERIFY(!map.contains("")); //should fail as key should not blank
}

//QSparqlQuery CopyConstructor() API property which ables to create copy constructor for QSparqlQuery
void tst_QSparqlQuery::copyConstructor()
{
    QSparqlConnection conn("QTRACKER");
    QVERIFY(conn.isValid());
    // check whether a valid query is inserted
    QSparqlQuery query("insert { <John> a nco:PersonContact; "
                       "nie:isLogicalPartOf <QTRACKER-database> ;"
                       "nco:nameGiven \"984649999\" .}",
                       QSparqlQuery::InsertStatement);
    QSparqlQuery copyquery(QSparqlQuery(query.query(),query.type()));

    QSparqlResult *result = conn.exec(copyquery);
    QVERIFY(result != 0);
    QVERIFY(result->hasError() == false);
    result->waitForFinished();
    QVERIFY(result->isFinished());
    QVERIFY(result->hasError() ==  false);
    QVERIFY(result->lastError().type() == QSparqlError::NoError);
    //Comparing both the queries
    QCOMPARE(copyquery.query(), query.query());
    QCOMPARE(copyquery.type(), query.type());
    delete result;
}

QTEST_MAIN( tst_QSparqlQuery )
#include "tst_qsparqlquery.moc"
