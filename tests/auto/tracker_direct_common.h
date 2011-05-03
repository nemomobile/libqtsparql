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

#ifndef QSPARQL_TRACKER_COMMON_H
#define QSPARQL_TRACKER_COMMON_H

#include <QtTest/QtTest>
#include <QtSparql/QtSparql>

class TrackerDirectCommon : public QObject
{
    Q_OBJECT

    public:
        TrackerDirectCommon();
        virtual ~TrackerDirectCommon();
        void installMsgHandler();
        void setMsgLogLevel(int logLevel);
        bool setupData();
        bool cleanData();

    private:
        QSparqlResult* runQuery(QSparqlConnection &conn, const QSparqlQuery &q, const QSparqlQueryOptions& options=QSparqlQueryOptions());
        virtual QSparqlResult* execQuery(QSparqlConnection &conn, const QSparqlQuery &q, const QSparqlQueryOptions& options=QSparqlQueryOptions()) =0;
        virtual void waitForQueryFinished(QSparqlResult* r) =0;
        virtual bool checkResultSize(QSparqlResult* r, int s) =0;

    private slots:
        void query_contacts();
        void insert_and_delete_contact();
        void query_with_error();
        void iterate_result();
        void iterate_result_rows();
        void iterate_result_bindings();
        void iterate_result_values();
        void iterate_result_stringValues();
        void special_chars();
        void data_types();
        void explicit_data_types();
        void large_integer();
        void datatype_string_data();
        void datatype_int_data();
        void datatype_double_data();
        void datatype_datetime_data();
        void datatype_boolean_data();
        void datatypes_as_properties_data();
        void datatypes_as_properties();
        void low_priority_updates();
};

class TestData : public QObject {
    Q_OBJECT
public:
    virtual ~TestData() { }
    virtual bool isOK() const =0;
    virtual QSparqlQuery selectQuery() const = 0;
};

TestData* createTestData(int testDataAmount, const QString& testSuiteTag, const QString& testCaseTag);

#endif // QSPARQL_TRACKER_COMMON_H
