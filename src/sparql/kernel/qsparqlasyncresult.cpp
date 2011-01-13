/****************************************************************************
**
** Copyright (C) 2010-2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (ivan.frade@nokia.com)
**
** This file is part of the QtSparql module (not yet part of the Qt Toolkit).
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at ivan.frade@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qvariant.h"
#include "qhash.h"
#include "qsparqlerror.h"
#include "qsparqlbinding.h"
#include "qsparqlresultrow.h"
#include "qsparqlasyncresult.h"
#include "qvector.h"
#include "qsparqldriver_p.h"
#include <QDebug>

QT_BEGIN_NAMESPACE

class QSparqlAsyncResultPrivate
{
public:
    QSparqlAsyncResultPrivate()
    : statementType(QSparqlQuery::SelectStatement), boolValue(false)
    {}

public:
    QString sparql;
    QSparqlQuery::StatementType statementType;
    QSparqlError error;
    bool boolValue;
};

/*!
    \class QSparqlAsyncResult

    \brief The QSparqlAsyncResult class provides an abstract interface for
    accessing the whole result set of of an executed QSparqlQuery in an
    asynchronous manner.

    \ingroup database
    \inmodule QtSparql

    When QSparqlConnection::execAsync() is called, it asynchronously begins the
    execution of the given query. The returned result is in an unfinished state
    so that isFinished() returns false. When the execution finished,
    QSparqlAsyncResult emits the finished() signal and sets itself to finished
    state.

    Retrieving the data is performed with the following functions:
    - binding()
    - value()
*/

/*!
    Creates a QSparqlAsyncResult. The result is empty, positioned at
    "before first row" and unfinished.

    \sa driver()
*/

QSparqlAsyncResult::QSparqlAsyncResult()
  : d (new QSparqlAsyncResultPrivate())
{
}

/*!
    Destroys the object and frees any allocated resources.
*/

QSparqlAsyncResult::~QSparqlAsyncResult()
{
}

/// Returns the query which was executed for creating this QSparqlAsyncResult. Useful
/// for debugging purposes.
QString QSparqlAsyncResult::query() const
{
    return d->sparql;
}

/// Sets the information about the query whose results this QSparqlAsyncResult object
/// represents.
void QSparqlAsyncResult::setQuery(const QString &query)
{
    d->sparql = query;
}

/// Sets the statement type of this QSparqlAsyncResult object.
void QSparqlAsyncResult::setStatementType(QSparqlQuery::StatementType type)
{
    d->statementType = type;
}

/// Returns true if this QSparqlAsyncResult object represents results that are in the
/// tabular format, e.g., the results of a SELECT query.
bool QSparqlAsyncResult::isTable() const
{
    return d->statementType == QSparqlQuery::SelectStatement;
}

/*!
    Returns true if the statement is a CONSTRUCT or DESCRIBE query
    returning a graph. Each QSparqlAsyncResultRow in a graph result hasError
    three QSParqlBinding values, named 's', 'p' and 'o' corresponding
    to triples with Subject, Predicate and Object values

    \sa isTable() isBool()
*/

bool QSparqlAsyncResult::isGraph() const
{
    return d->statementType == QSparqlQuery::ConstructStatement
            || d->statementType == QSparqlQuery::DescribeStatement;
}

/*!
    Returns true if the statement is an ASK query returning a
    boolean value

    \sa isTable() isGraph()
*/

/*!
    Returns true if the statement is an ASK query returning a
    boolean value

    \sa isTable() isGraph() boolValue()
*/

bool QSparqlAsyncResult::isBool() const
{
    return d->statementType == QSparqlQuery::AskStatement;
}

/*!
    Returns the boolean result of an ASK query

    \sa isBool() setBoolValue()
*/

bool QSparqlAsyncResult::boolValue() const
{
    return d->boolValue;
}

/*!
    Set the boolean result of an ASK query

    \sa isBool() boolValue()
*/
void QSparqlAsyncResult::setBoolValue(bool v)
{
    d->boolValue = v;
}

/*!
    Suspends the execution of the calling thread until all the query results
    have arrived. After this function returns, isFinished() should return true,
    indicating the result's contents are ready to be processed.

    \warning Calling this function from the main thread (the thread that
    calls QApplication::exec()) may cause your user interface to
    freeze.

    \sa isFinished()
*/
void QSparqlAsyncResult::waitForFinished()
{
}

/*!
    Returns true if the pending query has finished processing and the result has
    been received. If this function returns true, the hasError() and lastError()
    methods should return valid information.

    Note that this function only changes state if you call waitForFinished(), or
    if an external event happens, which in general only happens if you return to
    the event loop execution.

    \sa waitForFinished() lastError() error()
*/

bool QSparqlAsyncResult::isFinished() const
{
    return false;
}

/*!
  \fn int QSparqlAsyncResult::size() const

  Returns the size of the result (number of rows returned), or -1 if
  the size cannot be determined or if the database does not support
  reporting information about query sizes. If the query is not
  finished (isFinished() returns false), -1 is returned.

  \sa isFinished() QSparqlDriver::hasFeature()
*/

/*!
    \fn QSparqlBinding QSparqlAsyncResult::binding(int index) const

    Returns the binding \a index in the current result row.

    An invalid QSparqlBinding is returned if binding \a index does not
    exist.

    \sa value()
*/

/*!
    \fn QVariant QSparqlAsyncResult::value(int index) const

    Returns the value of binding \a index in the current result row.

    The binding values are numbered from left to right using the text of the
    \c SELECT statement, e.g. in

    \snippet doc/src/snippets/code/src_sparql_kernel_qsparqlquery.cpp 0

    field 0 is \c forename and field 1 is \c
    surname. Using \c{SELECT *} is not recommended because the order
    of the fields in the query is undefined.

    An invalid QVariant is returned if binding value \a index does not
    exist, if the query is inactive, or if the query is positioned on
    an invalid result row.

    \sa binding()
*/

/*!
    This function is provided for derived classes to set the last
    error to \a error.

    \sa lastError() hasError()
*/

void QSparqlAsyncResult::setLastError(const QSparqlError &error)
{
    d->error = error;
}

/*!
    Returns true if there is an error associated with the result.
    \sa setLastError() lastError()
*/

bool QSparqlAsyncResult::hasError() const
{
    return d->error.isValid();
}

/*!
    Returns the last error associated with the result.
*/

QSparqlError QSparqlAsyncResult::lastError() const
{
    return d->error;
}

/*!
    \fn void QSparqlAsyncResult::finished()

    This signal is emitted when the QSparqlAsyncResult has finished retrieving its
    data or when there was an error.
*/

/*!
    \fn void QSparqlAsyncResult::dataReady(int totalRows)

    This signal is emitted when a query has fetched data. The \a
    totalRows is the row count of the data set after the new data has
    arrived.
*/

QT_END_NAMESPACE
