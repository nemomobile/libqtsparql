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

class tst_QSparqlQueryModel : public QObject
{
    Q_OBJECT

public:
    tst_QSparqlQueryModel();
    virtual ~tst_QSparqlQueryModel();

public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:

    void setHeaderData();
    void indexInQuery();
    void insert_and_remove_columns();
    void resultRow();
};

tst_QSparqlQueryModel::tst_QSparqlQueryModel()
{
}

tst_QSparqlQueryModel::~tst_QSparqlQueryModel()
{
}

void tst_QSparqlQueryModel::initTestCase()
{
}

void tst_QSparqlQueryModel::cleanupTestCase()
{
}

void tst_QSparqlQueryModel::init()
{
}

void tst_QSparqlQueryModel::cleanup()
{
}

void tst_QSparqlQueryModel::setHeaderData()
{
}

void tst_QSparqlQueryModel::indexInQuery()
{
}

void tst_QSparqlQueryModel::insert_and_remove_columns()
{
}

void tst_QSparqlQueryModel::resultRow()
{
}

QTEST_MAIN( tst_QSparqlQueryModel )
#include "tst_qsparql_querymodel.moc"
