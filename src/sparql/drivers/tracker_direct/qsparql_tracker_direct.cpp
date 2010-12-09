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

#ifdef QSPARQL_LOG_RESULTS
QFile *resultsFile = 0;
#endif

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

class QTrackerDirectDriverPrivate {
public:
    QTrackerDirectDriverPrivate();
    ~QTrackerDirectDriverPrivate();

    TrackerSparqlConnection *connection;
    int dataReadyInterval;
};

class QTrackerDirectResultPrivate : public QObject {
    Q_OBJECT
public:
    QTrackerDirectResultPrivate(QTrackerDirectResult* result, QTrackerDirectDriverPrivate *dpp);

    ~QTrackerDirectResultPrivate();
    void terminate();
    void setLastError(const QSparqlError& e);
    void setBoolValue(bool v);
    void dataReady(int totalCount);

    TrackerSparqlCursor * cursor;
    QVector<QString> columnNames;
    QVector<QSparqlResultRow> results;
    bool isFinished;
    bool resultAlive; // whether the corresponding Result object is still alive
    QEventLoop *loop;

    QTrackerDirectResult* q;
    QTrackerDirectDriverPrivate *driverPrivate;
};

static void
async_cursor_next_callback( GObject *source_object,
                            GAsyncResult *result,
                            gpointer user_data)
{
    Q_UNUSED(source_object);
    QTrackerDirectResultPrivate *data = static_cast<QTrackerDirectResultPrivate*>(user_data);

    if (!data->resultAlive) {
        // The user has deleted the Result object before this callback was
        // called. Just delete the ResultPrivate here and do nothing.
        delete data;
        return;
    }
    GError *error = 0;
    gboolean active = tracker_sparql_cursor_next_finish(data->cursor, result, &error);

    if (error != 0) {
        QSparqlError e(QString::fromLatin1(error ? error->message : "unknown error"));
        e.setType(QSparqlError::BackendError);
        data->setLastError(e);
        g_error_free(error);
        data->terminate();
        return;
    }

    if (!active) {
        if (data->q->isBool()) {
            data->setBoolValue( data->results.count() == 1
                                && data->results[0].count() == 1
                                && data->results[0].binding(0).value().toBool());
        }

#ifdef QSPARQL_LOG_RESULTS
        resultsFile->write("  </results>\n");
        resultsFile->write("</sparql>");
        resultsFile->close();
#endif
        data->terminate();
        return;
    }

    QSparqlResultRow resultRow;
    gint n_columns = tracker_sparql_cursor_get_n_columns(data->cursor);

    if (data->columnNames.empty()) {
        for (int i = 0; i < n_columns; i++) {
            data->columnNames.append(QString::fromUtf8(tracker_sparql_cursor_get_variable_name(data->cursor, i)));
        }

#ifdef QSPARQL_LOG_RESULTS
        resultsFile->write("  <head>\n");

        for (int i = 0; i < n_columns; i++) {
            resultsFile->write( QByteArray("    <variable name=\"")
                                    + tracker_sparql_cursor_get_variable_name(data->cursor, i)
                                    + QByteArray("\"/>\n"));
        }

        resultsFile->write("  </head>\n");
        resultsFile->write("  <results>\n");
#endif
    }

    for (int i = 0; i < n_columns; i++) {
        QString value = QString::fromUtf8(tracker_sparql_cursor_get_string(data->cursor, i, 0));
        QSparqlBinding binding;
        binding.setName(data->columnNames[i]);
        TrackerSparqlValueType type = tracker_sparql_cursor_get_value_type(data->cursor, i);

        switch (type) {
        case TRACKER_SPARQL_VALUE_TYPE_UNBOUND:
            break;
        case TRACKER_SPARQL_VALUE_TYPE_URI:
            binding.setValue(QUrl(value));
            break;
        case TRACKER_SPARQL_VALUE_TYPE_STRING:
            binding.setValue(value);
            break;
        case TRACKER_SPARQL_VALUE_TYPE_INTEGER:
            binding.setValue(value, *XSD::Integer());
            break;
        case TRACKER_SPARQL_VALUE_TYPE_DOUBLE:
            binding.setValue(value, *XSD::Double());
            break;
        case TRACKER_SPARQL_VALUE_TYPE_DATETIME:
            binding.setValue(value, *XSD::DateTime());
            break;
        case TRACKER_SPARQL_VALUE_TYPE_BLANK_NODE:
            binding.setBlankNodeLabel(value);
            break;
        case TRACKER_SPARQL_VALUE_TYPE_BOOLEAN:
            if (value == QLatin1String("1") || value.toLower() == QLatin1String("true"))
                binding.setValue(QString::fromLatin1("true"), *XSD::Boolean());
            else
                binding.setValue(QString::fromLatin1("false"), *XSD::Boolean());
            break;
        default:
            break;
        }

        resultRow.append(binding);
    }

#ifdef QSPARQL_LOG_RESULTS
        resultsFile->write("    <result>\n");

        for (int i = 0; i < resultRow.count(); ++i) {
            resultsFile->write(QByteArray("      <binding name=\"") + data->columnNames[i].toLatin1() + QByteArray("\">\n"));
            if (resultRow.binding(i).isLiteral()) {
                resultsFile->write( QByteArray("        <literal datatype=\"")
                                            + resultRow.binding(i).dataTypeUri().toString().toLatin1()
                                            + QByteArray("\">")
                                            + resultRow.value(i).toString().toLatin1()
                                            + QByteArray("</literal>\n") );
            } else if (resultRow.binding(i).isUri()) {
                resultsFile->write( QByteArray("        <uri>")
                                            + resultRow.value(i).toString().toLatin1()
                                            + QByteArray("</uri>\n"));
            } else if (resultRow.binding(i).isBlank()) {
                resultsFile->write( QByteArray("        <bnode>")
                                            + resultRow.value(i).toString().toLatin1()
                                            + QByteArray("</bnode>\n"));
            }

            resultsFile->write("      </binding>\n");
        }

        resultsFile->write("    </result>\n");
#endif

    data->results.append(resultRow);
    if (data->results.count() % data->driverPrivate->dataReadyInterval == 0) {
        data->dataReady(data->results.count());
    }

    tracker_sparql_cursor_next_async(data->cursor, 0, async_cursor_next_callback, data);
}

static void
async_query_callback(   GObject *source_object,
                        GAsyncResult *result,
                        gpointer user_data)
{
    Q_UNUSED(source_object);
    QTrackerDirectResultPrivate *data = static_cast<QTrackerDirectResultPrivate*>(user_data);
    if (!data->resultAlive) {
        // The user has deleted the Result object before this callback was
        // called. Just delete the ResultPrivate here and do nothing.
        delete data;
        return;
    }
    GError *error = 0;
    data->cursor = tracker_sparql_connection_query_finish(data->driverPrivate->connection, result, &error);

    if (error != 0 || data->cursor == 0) {
        QSparqlError e(QString::fromLatin1(error ? error->message : "unknown error"));
        e.setType(QSparqlError::StatementError);
        data->setLastError(e);
        g_error_free(error);
        data->terminate();
        return;
    }

    tracker_sparql_cursor_next_async(data->cursor, 0, async_cursor_next_callback, data);
}

static void
async_update_callback( GObject *source_object,
                       GAsyncResult *result,
                       gpointer user_data)
{
    Q_UNUSED(source_object);
    QTrackerDirectResultPrivate *data = static_cast<QTrackerDirectResultPrivate*>(user_data);
    if (!data->resultAlive) {
        // The user has deleted the Result object before this callback was
        // called. Just delete the ResultPrivate here and do nothing.
        delete data;
        return;
    }
    GError *error = 0;
    tracker_sparql_connection_update_finish(data->driverPrivate->connection, result, &error);

    if (error != 0) {
        QSparqlError e(QString::fromLatin1(error ? error->message : "unknown error"));
        e.setType(QSparqlError::StatementError);
        data->setLastError(e);
        g_error_free(error);
        data->terminate();
        return;
    }

    data->terminate();
}

QTrackerDirectResultPrivate::QTrackerDirectResultPrivate(
    QTrackerDirectResult* result,
    QTrackerDirectDriverPrivate *dpp)
: cursor(0), isFinished(false), resultAlive(true), loop(0),
  q(result), driverPrivate(dpp)
{
}

QTrackerDirectResultPrivate::~QTrackerDirectResultPrivate()
{
    if (cursor != 0)
        g_object_unref(cursor);
}

void QTrackerDirectResultPrivate::terminate()
{
    if (results.count() % driverPrivate->dataReadyInterval != 0) {
        dataReady(results.count());
    }

    isFinished = true;
    q->emit finished();
    if (cursor != 0) {
        g_object_unref(cursor);
        cursor = 0;
    }
    if (loop != 0)
        loop->exit();
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

QTrackerDirectResult::QTrackerDirectResult(QTrackerDirectDriverPrivate* p)
{
    d = new QTrackerDirectResultPrivate(this, p);
}

QTrackerDirectResult::~QTrackerDirectResult()
{
    if (!d->isFinished) {
        // We're deleting the Result before the async data retrieval has
        // finished. There is a pending async callback that will be called at
        // some point, and that will have our ResultPrivate as user_data. Don't
        // delete the ResultPrivate here, but just mark that we're no longer
        // alive. The callback will then delete the ResultPrivate.
        d->resultAlive = false;
    }
    else {
        delete d;
    }
}

QTrackerDirectResult* QTrackerDirectDriver::exec(const QString& query,
                          QSparqlQuery::StatementType type)
{
    QTrackerDirectResult* res = new QTrackerDirectResult(d);
    res->setStatementType(type);
    res->setQuery(query);

    switch (type) {
    case QSparqlQuery::AskStatement:
    case QSparqlQuery::SelectStatement:
    {
#ifdef QSPARQL_LOG_RESULTS
        QString resultsFilename = QString::fromLatin1(getenv("QSPARQL_DUMMY_RESULTS"));
        if (resultsFilename.isEmpty()) {
            resultsFile = new QTemporaryFile(QDir::tempPath() + QLatin1String("/sparql-results"));
        } else {
            resultsFile =  new QFile(resultsFilename);
        }

        resultsFile->open(QIODevice::WriteOnly);
        resultsFile->write("<?xml version=\"1.0\"?>\n");
        resultsFile->write("<sparql xmlns=\"http://www.w3.org/2005/sparql-results#\">\n");
#endif
        tracker_sparql_connection_query_async(  d->connection,
                                                query.toLatin1().constData(),
                                                0,
                                                async_query_callback,
                                                res->d);
        break;
    }
    case QSparqlQuery::InsertStatement:
    case QSparqlQuery::DeleteStatement:
        tracker_sparql_connection_update_async( d->connection,
                                                query.toLatin1().constData(),
                                                0,
                                                0,
                                                async_update_callback,
                                                res->d);
    {
        break;
    }
    default:
        qWarning() << "Tracker backend: unsupported statement type";
        res->setLastError(QSparqlError(
                              QLatin1String("Unsupported statement type"),
                              QSparqlError::BackendError));
        break;
    }
    return res;
}

void QTrackerDirectResult::cleanup()
{
    setPos(QSparql::BeforeFirstRow);
}

QSparqlBinding QTrackerDirectResult::binding(int field) const
{
    if (!isValid()) {
        return QSparqlBinding();
    }

    if (field >= d->results[pos()].count() || field < 0) {
        qWarning() << "QTrackerDirectResult::data[" << pos() << "]: column" << field << "out of range";
        return QSparqlBinding();
    }

    return d->results[pos()].binding(field);
}

QVariant QTrackerDirectResult::value(int field) const
{
    if (!isValid()) {
        return QVariant();
    }

    if (field >= d->results[pos()].count() || field < 0) {
        qWarning() << "QTrackerDirectResult::data[" << pos() << "]: column" << field << "out of range";
        return QVariant();
    }

    return d->results[pos()].value(field);
}

void QTrackerDirectResult::waitForFinished()
{
    if (d->isFinished)
        return;

    QEventLoop loop;
    d->loop = &loop;
    loop.exec();
    d->loop = 0;
}

bool QTrackerDirectResult::isFinished() const
{
    return d->isFinished;
}

int QTrackerDirectResult::size() const
{
    return d->results.count();
}

QSparqlResultRow QTrackerDirectResult::current() const
{
    if (!isValid()) {
        return QSparqlResultRow();
    }

    if (pos() < 0 || pos() >= d->results.count())
        return QSparqlResultRow();

    return d->results[pos()];
}

QTrackerDirectDriverPrivate::QTrackerDirectDriverPrivate()
    : dataReadyInterval(1)
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
    d->dataReadyInterval = options.dataReadyInterval();

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
    g_object_unref(d->connection);

    if (isOpen()) {
        setOpen(false);
        setOpenError(false);
    }
}

QT_END_NAMESPACE

#include "qsparql_tracker_direct.moc"
