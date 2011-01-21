/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <tracker-sparql.h>

#include "qsparql_tracker_direct_p.h"

#include <qsparqlerror.h>
#include <qsparqlbinding.h>
#include <qsparqlquery.h>
#include <qsparqlresultrow.h>
#include <qsparqlconnection.h>

#include <qcoreapplication.h>
#include <qvariant.h>
#include <qstringlist.h>
#include <qvector.h>

#include <qdebug.h>

QT_BEGIN_NAMESPACE

namespace XSD {
Q_GLOBAL_STATIC_WITH_ARGS(QUrl, Boolean,
                          (QLatin1String("http://www.w3.org/2001/XMLSchema#boolean")))

Q_GLOBAL_STATIC_WITH_ARGS(QUrl, DateTime,
                          (QLatin1String("http://www.w3.org/2001/XMLSchema#dateTime")))

Q_GLOBAL_STATIC_WITH_ARGS(QUrl, Double,
                          (QLatin1String("http://www.w3.org/2001/XMLSchema#double")))

Q_GLOBAL_STATIC_WITH_ARGS(QUrl, Integer,
                          (QLatin1String("http://www.w3.org/2001/XMLSchema#integer")))
}

static QSparqlBinding qMakeBinding(TrackerSparqlValueType type, const QByteArray &value, const QString& columnName)
{
    QSparqlBinding binding;
    binding.setName(columnName);

    switch (type) {
    case TRACKER_SPARQL_VALUE_TYPE_UNBOUND:
        break;
    case TRACKER_SPARQL_VALUE_TYPE_URI:
        binding.setValue(QUrl::fromEncoded(value));
        break;
    case TRACKER_SPARQL_VALUE_TYPE_STRING:
    {
        binding.setValue(QString::fromUtf8(value));
        break;
    }
    case TRACKER_SPARQL_VALUE_TYPE_INTEGER:
    {
        binding.setValue(QString::fromUtf8(value), *XSD::Integer());
        break;
    }
    case TRACKER_SPARQL_VALUE_TYPE_DOUBLE:
    {
        binding.setValue(QString::fromUtf8(value), *XSD::Double());
        break;
    }
    case TRACKER_SPARQL_VALUE_TYPE_DATETIME:
    {
        binding.setValue(QString::fromUtf8(value), *XSD::DateTime());
        break;
    }
    case TRACKER_SPARQL_VALUE_TYPE_BLANK_NODE:
    {
        binding.setBlankNodeLabel(QString::fromUtf8(value));
        break;
    }
    case TRACKER_SPARQL_VALUE_TYPE_BOOLEAN:
    {
        if (value == "1" || value.toLower() == "true")
            binding.setValue(QString::fromLatin1("true"), *XSD::Boolean());
        else
            binding.setValue(QString::fromLatin1("false"), *XSD::Boolean());
        break;
    }
    default:
        break;
    }

    return binding;
}

class QTrackerDirectFetcherPrivate : public QThread
{
public:
    QTrackerDirectFetcherPrivate(QTrackerDirectResult *res) : result(res) { }

    void run()
    {
        setTerminationEnabled(false);
        if (result->runQuery()) {
            if (result->isTable()) {
                while (result->fetchNextResult()) {
                    setTerminationEnabled(true);
                    setTerminationEnabled(false);
                }
            } else if (result->isBool()) {
                result->fetchBoolResult();
            } else {
                result->terminate();
            }
        }
        setTerminationEnabled(true);
    }

private:
    QTrackerDirectResult *result;
};

class QTrackerDirectDriverPrivate {
public:
    QTrackerDirectDriverPrivate();
    ~QTrackerDirectDriverPrivate();

    TrackerSparqlConnection *connection;
    int dataReadyInterval;
    int dataReadyBufferSize;
    // This mutex is for ensuring that only one thread at a time
    // is using the connection to make tracker queries. This mutex
    // probably isn't needed as a TrackerSparqlConnection is
    // already thread safe.
    QMutex mutex;
};

class QTrackerDirectResultPrivate : public QObject {
    Q_OBJECT
public:
    QTrackerDirectResultPrivate(QTrackerDirectResult* result, QTrackerDirectDriverPrivate *dpp, QTrackerDirectFetcherPrivate *f);

    ~QTrackerDirectResultPrivate();
    int resultsCount();
    int resultsPos();
    void setFreeResults();
    void terminate();
    void setLastError(const QSparqlError& e);
    void setBoolValue(bool v);
    void dataReady(int totalCount);

    TrackerSparqlCursor* cursor;
    QVector<QString> columnNames;
    QList<QVector<QPair<TrackerSparqlValueType, QByteArray> > > results;

    // These two fields are only used by the dataReadyBufferSize option
    //  - resultsBase: count of the number of results deleted
    //  - freeResults: number of free entries in the results buffer,
    //      and the fetcher thread will wait until it is at least 1
    int resultsBase;
    QSemaphore freeResults;

    bool isFinished;

    QTrackerDirectResult* q;
    QTrackerDirectDriverPrivate *driverPrivate;
    QTrackerDirectFetcherPrivate *fetcher;
    bool fetcherStarted;
    // This mutex is for ensuring that only one thread at a time is accessing
    // the member variables of this class (mainly: "results" and "cursor").
    QMutex mutex;
};

QTrackerDirectResultPrivate::QTrackerDirectResultPrivate(   QTrackerDirectResult* result,
                                                            QTrackerDirectDriverPrivate *dpp,
                                                            QTrackerDirectFetcherPrivate *f)
  : cursor(0), resultsBase(0), isFinished(false),
  q(result), driverPrivate(dpp), fetcher(f), fetcherStarted(false),
  mutex(QMutex::Recursive)
{
}

QTrackerDirectResultPrivate::~QTrackerDirectResultPrivate()
{
    // The fetcher thread also accesses "cursor", so, first terminate the
    // thread, and only after that unref the cursor. Don't lock the mutex here,
    // since the thread might lock the same mutex right after us, and we
    // couldn't terminate it then.
    if (fetcher->isRunning()) {
        fetcher->terminate();
        // This might (in theory) block forever. But the fetcher thread *should*
        // enter a "termination enabled" point after fetching each row.
        fetcher->wait();
    }

    delete fetcher;

    QMutexLocker resultLocker(&mutex);
    if (cursor != 0) {
        g_object_unref(cursor);
        cursor = 0;
    }
}

int QTrackerDirectResultPrivate::resultsCount()
{
    return resultsBase + results.count();
}

int QTrackerDirectResultPrivate::resultsPos()
{
    return q->pos() - resultsBase;
}

void QTrackerDirectResultPrivate::setFreeResults()
{
    if (driverPrivate->dataReadyBufferSize > 0)
        freeResults.release(driverPrivate->dataReadyBufferSize - resultsPos() - 1);
}

void QTrackerDirectResultPrivate::terminate()
{
    QMutexLocker resultLocker(&mutex);

    if (results.count() % driverPrivate->dataReadyInterval != 0) {
        dataReady(results.count());
    }

    isFinished = true;
    q->emit finished();
    if (cursor != 0) {
        g_object_unref(cursor);
        cursor = 0;
    }
}

void QTrackerDirectResultPrivate::setLastError(const QSparqlError& e)
{
    q->setLastError(e);
}

void QTrackerDirectResultPrivate::setBoolValue(bool v)
{
    q->setBoolValue(v);
}

void QTrackerDirectResultPrivate::dataReady(int totalCount)
{
    emit q->dataReady(totalCount);
}

QTrackerDirectResult::QTrackerDirectResult(QTrackerDirectDriverPrivate* p,
                                           const QString& query,
                                           QSparqlQuery::StatementType type)
{
    setQuery(query);
    setStatementType(type);
    d = new QTrackerDirectResultPrivate(this, p, new QTrackerDirectFetcherPrivate(this));
}

QTrackerDirectResult::~QTrackerDirectResult()
{
    delete d;
}

QTrackerDirectResult* QTrackerDirectDriver::exec(const QString& query, QSparqlQuery::StatementType type)
{
    QTrackerDirectResult* res = new QTrackerDirectResult(d, query, type);

    if (type != QSparqlQuery::AskStatement
        && type != QSparqlQuery::SelectStatement
        && type != QSparqlQuery::InsertStatement
        && type != QSparqlQuery::DeleteStatement)
    {
        setLastError(QSparqlError(
                              QLatin1String("Unsupported statement type"),
                              QSparqlError::BackendError));
        qWarning() << "QTrackerDirectResult:" << lastError() << query;
        res->terminate();
    }
    else {
        // Queue calling exec() on the result. This way the finished() and
        // dataReady() signals won't be emitted before the user connects to
        // them, and the result won't be in the "finished" state before the
        // thread that calls this function has entered its event loop.
        QMetaObject::invokeMethod(res, "startFetcher",  Qt::QueuedConnection);
    }
    return res;
}

void QTrackerDirectResult::startFetcher()
{
    QMutexLocker resultLocker(&(d->mutex));
    if (!d->fetcherStarted) {
        d->fetcherStarted = true;
        d->fetcher->start();
    }
}

bool QTrackerDirectResult::runQuery()
{
    QMutexLocker connectionLocker(&(d->driverPrivate->mutex));
    QMutexLocker resultLocker(&(d->mutex));

    GError * error = 0;
    if (isTable() || isBool()) {
        d->cursor = tracker_sparql_connection_query(    d->driverPrivate->connection,
                                                        query().toUtf8().constData(),
                                                        0,
                                                        &error );
        if (error != 0 || d->cursor == 0) {
            QSparqlError e(QString::fromLatin1(error ? error->message : "unknown error"),
                           QSparqlError::StatementError,
                           error ? error->code : -1);
            setLastError(e);
            if (error)
                g_error_free(error);
            qWarning() << "QTrackerDirectResult:" << lastError() << query();
            terminate();
            return false;
        }
    } else {
        tracker_sparql_connection_update(   d->driverPrivate->connection,
                                            query().toUtf8().constData(),
                                            0,
                                            0,
                                            &error );
        if (error != 0) {
            QSparqlError e(QString::fromLatin1(error->message),
                           QSparqlError::StatementError,
                           error->code);
            g_error_free(error);
            setLastError(e);
            qWarning() << "QTrackerDirectResult:" << lastError() << query();
            terminate();
            return false;
        }
    }

    return true;
}

void QTrackerDirectResult::cleanup()
{
    setPos(QSparql::BeforeFirstRow);
}

bool QTrackerDirectResult::fetchNextResult()
{
    QMutexLocker connectionLocker(&(d->driverPrivate->mutex));

    GError * error = 0;
    gboolean active = tracker_sparql_cursor_next(d->cursor, 0, &error);

    if (error != 0) {
        QSparqlError e(QString::fromLatin1(error->message),
                       QSparqlError::BackendError,
                       error->code);
        g_error_free(error);
        setLastError(e);
        qWarning() << "QTrackerDirectResult:" << lastError() << query();
        terminate();
        return false;
    }

    if (!active) {
        terminate();
        return false;
    }

    if (d->driverPrivate->dataReadyBufferSize > 0) {
        d->freeResults.acquire(1);
    }

    QMutexLocker resultLocker(&(d->mutex));

    if (d->driverPrivate->dataReadyBufferSize > 0) {
        if ((d->results.count() + 1) > d->driverPrivate->dataReadyBufferSize) {
            d->results.removeFirst();
            d->resultsBase++;
        }
    }

    QVector<QPair<TrackerSparqlValueType, QByteArray> > resultRow;
    gint n_columns = tracker_sparql_cursor_get_n_columns(d->cursor);

    if (d->columnNames.empty()) {
        for (int i = 0; i < n_columns; i++) {
            d->columnNames.append(QString::fromUtf8(tracker_sparql_cursor_get_variable_name(d->cursor, i)));
        }
    }

    for (int i = 0; i < n_columns; i++) {
        QPair<TrackerSparqlValueType, QByteArray> value(tracker_sparql_cursor_get_value_type(d->cursor, i),
                                                      tracker_sparql_cursor_get_string(d->cursor, i, 0) );
        resultRow.append(value);
    }

    d->results.append(resultRow);
    if (d->resultsCount() % d->driverPrivate->dataReadyInterval == 0) {
        d->dataReady(d->resultsCount());
    }

    return true;
}

bool QTrackerDirectResult::fetchBoolResult()
{
    QMutexLocker connectionLocker(&(d->driverPrivate->mutex));
    QMutexLocker resultLocker(&(d->mutex));

    GError * error = 0;
    tracker_sparql_cursor_next(d->cursor, 0, &error);
    if (error != 0) {
        QSparqlError e(QString::fromLatin1(error->message),
                       QSparqlError::BackendError,
                       error->code);
        g_error_free(error);
        setLastError(e);
        qWarning() << "QTrackerDirectResult:" << lastError() << query();
        terminate();
        return false;
    }

    if (tracker_sparql_cursor_get_n_columns(d->cursor) == 1)  {
        QString value = QString::fromUtf8(tracker_sparql_cursor_get_string(d->cursor, 0, 0));
        TrackerSparqlValueType type = tracker_sparql_cursor_get_value_type(d->cursor, 0);

        if (    type == TRACKER_SPARQL_VALUE_TYPE_BOOLEAN
                && (value == QLatin1String("1") || value.toLower() == QLatin1String("true")) )
        {
            d->setBoolValue(true);
        }
    }

    terminate();
    return true;
}

QSparqlBinding QTrackerDirectResult::binding(int field) const
{
    QMutexLocker resultLocker(&(d->mutex));
    d->setFreeResults();

    if (!isValid()) {
        return QSparqlBinding();
    }

    if (field >= d->results[d->resultsPos()].count() || field < 0) {
        qWarning() << "QTrackerDirectResult::data[" << pos() << "]: column" << field << "out of range";
        return QSparqlBinding();
    }

    return qMakeBinding(    d->results[d->resultsPos()][field].first,
                            d->results[d->resultsPos()][field].second,
                            d->columnNames[field]);
}

QVariant QTrackerDirectResult::value(int field) const
{
    QMutexLocker resultLocker(&(d->mutex));
    d->setFreeResults();

    if (!isValid()) {
        return QVariant();
    }

    if (field >= d->results[d->resultsPos()].count() || field < 0) {
        qWarning() << "QTrackerDirectResult::data[" << pos() << "]: column" << field << "out of range";
        return QVariant();
    }

    QSparqlBinding binding = qMakeBinding(  d->results[d->resultsPos()][field].first,
                                            d->results[d->resultsPos()][field].second,
                                            d->columnNames[field]);
    return binding.value();
}

QString QTrackerDirectResult::string(int field) const
{
    QMutexLocker resultLocker(&(d->mutex));
    d->setFreeResults();

    if (!isValid()) {
        return QString();
    }

    if (field >= d->results[d->resultsPos()].count() || field < 0) {
        qWarning() << "QTrackerDirectResult::data[" << pos() << "]: column" << field << "out of range";
        return QString();
    }

    return QString::fromUtf8(d->results[d->resultsPos()][field].second);
}

void QTrackerDirectResult::waitForFinished()
{
    if (d->isFinished)
        return;

    // We may have queued startFetcher, call it now.
    startFetcher();

    d->fetcher->wait();
}

bool QTrackerDirectResult::isFinished() const
{
    return d->isFinished;
}

void QTrackerDirectResult::terminate()
{
    d->terminate();
}

int QTrackerDirectResult::size() const
{
    QMutexLocker resultLocker(&(d->mutex));
    return d->resultsCount();
}

QSparqlResultRow QTrackerDirectResult::current() const
{
    QMutexLocker resultLocker(&(d->mutex));
    d->setFreeResults();

    if (!isValid()) {
        return QSparqlResultRow();
    }

    if (d->resultsPos() < 0 || pos() >= d->resultsCount())
        return QSparqlResultRow();

    QSparqlResultRow resultRow;
    for (int i = 0; i < d->columnNames.count(); ++i) {
        resultRow.append(qMakeBinding(d->results[d->resultsPos()][i].first,
                                      d->results[d->resultsPos()][i].second,
                                      d->columnNames[i]));
    }
    return resultRow;
}

QTrackerDirectDriverPrivate::QTrackerDirectDriverPrivate()
    : mutex(QMutex::Recursive)
{
}

QTrackerDirectDriverPrivate::~QTrackerDirectDriverPrivate()
{
}

QTrackerDirectDriver::QTrackerDirectDriver(QObject* parent)
    : QSparqlDriver(parent)
{
    d = new QTrackerDirectDriverPrivate();
    /* Initialize GLib type system */
    g_type_init();

}

QTrackerDirectDriver::~QTrackerDirectDriver()
{
    delete d;
}

bool QTrackerDirectDriver::hasFeature(QSparqlConnection::Feature f) const
{
    switch (f) {
    case QSparqlConnection::QuerySize:
        return true;
    case QSparqlConnection::AskQueries:
        return true;
    case QSparqlConnection::ConstructQueries:
        return false;
    case QSparqlConnection::UpdateQueries:
        return true;
    case QSparqlConnection::DefaultGraph:
        return true;
    }
    return false;
}

QAtomicInt connectionCounter = 0;

bool QTrackerDirectDriver::open(const QSparqlConnectionOptions& options)
{
    QMutexLocker connectionLocker(&(d->mutex));

    d->dataReadyInterval = options.dataReadyInterval();
    d->dataReadyBufferSize = options.dataReadyBufferSize();

    if (isOpen())
        close();

    GError *error = 0;
    d->connection = tracker_sparql_connection_get(0, &error);
    // maybe-TODO: Add the GCancellable
    if (!d->connection) {
        qWarning("Couldn't obtain a direct connection to the Tracker store: %s",
                    error ? error->message : "unknown error");
        g_error_free(error);
        return false;
    }

    setOpen(true);
    setOpenError(false);

    return true;
}

void QTrackerDirectDriver::close()
{
    QMutexLocker connectionLocker(&(d->mutex));
    g_object_unref(d->connection);

    if (isOpen()) {
        setOpen(false);
        setOpenError(false);
    }
}

QT_END_NAMESPACE

#include "qsparql_tracker_direct.moc"
