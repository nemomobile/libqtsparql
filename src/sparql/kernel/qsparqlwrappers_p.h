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

#ifndef QSPARQLWRAPPERS_H
#define QSPARQLWRAPPERS_H

#include <QtSparql/qsparqlasyncresult.h>

#include <QScopedPointer>

class QSparqlAsyncWrapperPrivate;
class QSparqlBinding;
class QSparqlSyncIterator;
class QVariant;

class QSparqlAsyncWrapper : public QSparqlAsyncResult
{
    Q_OBJECT
public:
    explicit QSparqlAsyncWrapper(QSparqlSyncIterator* backend,
                                 int dataReadyInterval);
    ~QSparqlAsyncWrapper();
    virtual QVariant value(int row, int col) const;
    virtual QSparqlBinding binding(int row, int col) const;
    virtual int size() const;

    virtual void waitForFinished();
    virtual bool isFinished() const;

private:
    QScopedPointer<QSparqlAsyncWrapperPrivate> d;

    friend class FetcherThread;
    friend class QSparqlAsyncWrapperPrivate;
};

#endif
