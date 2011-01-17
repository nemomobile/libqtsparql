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

#include "qsparqlwrappers_p.h"

#include <QtSparql/qsparqlbinding.h>
#include <QtSparql/qsparqlsynciterator.h>

#include <QThread>
#include <QDebug>
#include <QMutex>
#include <QMutexLocker>

class QSparqlAsyncWrapper;

class FetcherThread : public QThread
{
public:
    FetcherThread(QSparqlAsyncWrapper *wrapper) : wrapper(wrapper) { }

    void run();

private:
    QSparqlAsyncWrapper *wrapper;
};

class QSparqlAsyncWrapperPrivate
{
public:
    QSparqlAsyncWrapperPrivate(QSparqlSyncIterator* backend,
                               int dataReadyInterval,
                               QSparqlAsyncWrapper* p);
    ~QSparqlAsyncWrapperPrivate();

    bool fetchNextResult();
    void fetchBoolResult();
    void terminate();
    QScopedPointer<QSparqlSyncIterator> backend;
    int dataReadyInterval;
    QSparqlAsyncWrapper* q;
    QMutex resultMutex;
    QVector<QSparqlResultRow> results;
    FetcherThread fetcher;
    bool isFinished;
};

void FetcherThread::run()
{
    setTerminationEnabled(false);
    if (wrapper->isTable()) {
        while (wrapper->d->fetchNextResult()) {
            setTerminationEnabled(true);
            setTerminationEnabled(false);
        }
    } else if (wrapper->isBool()) {
        wrapper->d->fetchBoolResult();
    } else {
        wrapper->d->terminate();
    }
    setTerminationEnabled(true);
}

QSparqlAsyncWrapperPrivate::QSparqlAsyncWrapperPrivate(
    QSparqlSyncIterator* backend, int dataReadyInterval, QSparqlAsyncWrapper* q)
    : backend(backend), // will take the ownership
      dataReadyInterval(dataReadyInterval),
      q(q),
      fetcher(q),
      isFinished(false)
{
    fetcher.start();
}

QSparqlAsyncWrapperPrivate::~QSparqlAsyncWrapperPrivate()
{
    if (fetcher.isRunning()) {
        fetcher.terminate();
        qWarning() << "QSparqlAsyncResult: "
                   << "unable to terminate the result fetcher thread "
                   << "(should never happen)";
    }
}

bool QSparqlAsyncWrapperPrivate::fetchNextResult()
{
    if (backend->next()) {
        QMutexLocker resultLocker(&resultMutex);
        // TODO: column names need to come from somewhere
        QSparqlResultRow resultRow;
        for (int i = 0; i < backend->count(); ++i) {
            QSparqlBinding binding;
            binding.setValue(backend->value(i));
            resultRow.append(binding);
        }
        results.append(resultRow);

        if (results.size() % dataReadyInterval == 0) {
            emit q->dataReady(results.size());
        }

        return true;
    }
    else {
        terminate();
        return false;
    }
}

void QSparqlAsyncWrapperPrivate::fetchBoolResult()
{
    if (backend->next()) {
        QMutexLocker resultLocker(&resultMutex);
        // TODO: column names need to come from somewhere
        if (backend->value(0).toBool())
            q->setBoolValue(true);
    }
    terminate();
}

void QSparqlAsyncWrapperPrivate::terminate()
{
    if (results.size() % dataReadyInterval != 0) {
        // If it is 0, dataReady has already been emitted once
        emit q->dataReady(results.size());
    }

    isFinished = true;
    emit q->finished();
}

QSparqlAsyncWrapper::QSparqlAsyncWrapper(QSparqlSyncIterator* backend,
                                         int dataReadyInterval)
    : d(new QSparqlAsyncWrapperPrivate(backend, dataReadyInterval, this))
{
}

QSparqlAsyncWrapper::~QSparqlAsyncWrapper()
{
}

QVariant QSparqlAsyncWrapper::value(int row, int col) const
{
    QMutexLocker resultLocker(&(d->resultMutex));
    if (row < 0 || row >= d->results.size() || col < 0 || col >= d->results[row].count()) {
        qWarning() << "QSparqlAsyncResult::value(" << row << ", " << col << ") out of range";
        return QVariant();
    }
    return d->results[row].value(col);
}

QSparqlBinding QSparqlAsyncWrapper::binding(int row, int col) const
{
    QMutexLocker resultLocker(&(d->resultMutex));
    if (row < 0 || row >= d->results.size() || col < 0 || col >= d->results[row].count()) {
        qWarning() << "QSparqlAsyncResult::value(" << row << ", " << col << ") out of range";
        return QSparqlBinding();
    }
    return d->results[row].binding(col);
}

int QSparqlAsyncWrapper::size() const
{
    QMutexLocker locker(&d->resultMutex);
    return d->results.size();
}

void QSparqlAsyncWrapper::waitForFinished()
{
    if (d->isFinished)
        return;
    d->fetcher.wait();
}

bool QSparqlAsyncWrapper::isFinished() const
{
    return d->isFinished;
}
