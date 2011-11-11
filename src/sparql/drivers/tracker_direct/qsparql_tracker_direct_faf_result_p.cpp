/****************************************************************************
**
** Copyright (C) 2010-2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (ivan.frade@nokia.com)
**
** This file is part of the QtSparql module (not yet part of the Qt Toolkit).
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

#include "qsparql_tracker_direct_faf_result_p.h"
#include "qsparql_tracker_direct_p.h"
#include "qsparql_tracker_direct_driver_p.h"

#include <QtSparql/qsparqlerror.h>
#include <QtSparql/qsparqlbinding.h>
#include <QtSparql/qsparqlquery.h>
#include <QtSparql/qsparqlqueryoptions.h>
#include <QtSparql/qsparqlresultrow.h>

#include <QtCore/qvariant.h>
#include <QtCore/qdebug.h>

QT_BEGIN_NAMESPACE

QTrackerDirectFAFResult::QTrackerDirectFAFResult(QTrackerDirectDriverPrivate* p,
                                                   const QString& query,
                                                   QSparqlQuery::StatementType type,
                                                   const QSparqlQueryOptions& options)
    : QTrackerDirectResult(options)
{
    setQuery(query);
    setStatementType(type);
    driverPrivate = p;
}

QTrackerDirectFAFResult::~QTrackerDirectFAFResult()
{
    stopAndWait();
}

void QTrackerDirectFAFResult::terminate()
{
    resultFinished = 1;
}

void QTrackerDirectFAFResult::exec()
{
    runQuery();
}

void QTrackerDirectFAFResult::runQuery()
{
    if (statementType() == QSparqlQuery::InsertStatement || statementType() == QSparqlQuery::DeleteStatement) {
        updateQuery();
    }
}

void QTrackerDirectFAFResult::updateQuery()
{
    if (!driverPrivate->driver->isOpen()) {
        setLastError(QSparqlError(driverPrivate->error,
                                  QSparqlError::ConnectionError));
        return;
    }
    tracker_sparql_connection_update_async(driverPrivate->connection,
                                           query().toUtf8().constData(),
                                           qSparqlPriorityToGlib(options.priority()),
                                           0,
                                           0,
                                           0);
    terminate();
}

QSparqlResultRow QTrackerDirectFAFResult::current() const
{
    return QSparqlResultRow();
}

QSparqlBinding QTrackerDirectFAFResult::binding(int i) const
{
    return QSparqlBinding();

}

QVariant QTrackerDirectFAFResult::value(int i) const
{
    return QVariant();
}

void QTrackerDirectFAFResult::stopAndWait()
{
}

bool QTrackerDirectFAFResult::isFinished() const
{
    return true;
}

bool QTrackerDirectFAFResult::hasFeature(QSparqlResult::Feature feature) const
{
    switch (feature) {
    case QSparqlResult::Sync:
    case QSparqlResult::ForwardOnly:
        return true;
    case QSparqlResult::QuerySize:
        return false;
    default:
        return false;
    }
}

QT_END_NAMESPACE
