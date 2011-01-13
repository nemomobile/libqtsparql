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

#ifndef QSPARQLASYNCRESULT_H
#define QSPARQLASYNCRESULT_H

#include <QtCore/qvariant.h>
#include <QtCore/qobject.h>
#include <QtSparql/qsparqlresultrow.h>
#include <QtSparql/qsparqlquery.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Sparql)

class QSparqlAsyncResultPrivate;
class QSparqlError;

class Q_SPARQL_EXPORT QSparqlAsyncResult : public QObject
{
    Q_OBJECT
public:
    virtual ~QSparqlAsyncResult();

    // Accessing the data
    virtual QVariant value(int row, int col) const = 0;
    virtual QSparqlBinding binding(int row, int col) const = 0;

    bool isTable() const;
    bool isGraph() const;
    bool isBool() const;

    // For ASK results
    bool boolValue() const;

    virtual int size() const = 0;

    virtual void waitForFinished();
    virtual bool isFinished() const;

    bool hasError() const;
    QSparqlError lastError() const;

    QString query() const;

Q_SIGNALS:
    void finished();
    void dataReady(int totalRows);

protected:
    QSparqlAsyncResult();
    void setQuery(const QString & query);
    void setStatementType(QSparqlQuery::StatementType type);
    virtual void setLastError(const QSparqlError& e);
    void setBoolValue(bool v);

private:
    QScopedPointer<QSparqlAsyncResultPrivate> d;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QSPARQLASYNCRESULT_H
