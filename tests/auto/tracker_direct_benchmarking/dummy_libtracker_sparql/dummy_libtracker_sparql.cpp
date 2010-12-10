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

#include <dummy_libtracker_sparql.h>

#include <QtCore/qdebug.h>

#include <QtCore/qfile.h>
#include <QtCore/qstringlist.h>
#include <QtCore/qvector.h>

#include <QtXml/QDomNode>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>

static GAsyncReadyCallback async_cursor_next_callback = 0;
static gpointer async_cursor_next_user_data = 0;
static GAsyncResult *async_cursor_result = 0;

static GAsyncReadyCallback async_query_callback = 0;
static gpointer async_query_user_data = 0;
static GAsyncResult *async_query_result = 0;

static GAsyncReadyCallback async_update_callback = 0;
static gpointer async_update_user_data = 0;
static GAsyncResult *async_update_result = 0;

static QStringList variableNames;
static QStringList variableValues;
static QVector<TrackerSparqlValueType> variableDataTypes;

static gpointer tracker_sparql_connection_parent_class = NULL;

static void tracker_sparql_connection_instance_init (TrackerSparqlConnection * self) {
}

static void tracker_sparql_connection_real_init (TrackerSparqlConnection* self, GError** error) {
    g_return_if_fail (self != NULL);
    g_warning ("tracker-connection.vala:328: Interface 'init' not implemented");
}

static void tracker_sparql_connection_finalize (GObject* obj) {
    TrackerSparqlConnection * self;
    self = TRACKER_SPARQL_CONNECTION (obj);
    G_OBJECT_CLASS (tracker_sparql_connection_parent_class)->finalize (obj);
}

static void tracker_sparql_connection_class_init (TrackerSparqlConnectionClass * klass) {
    tracker_sparql_connection_parent_class = g_type_class_peek_parent (klass);
    TRACKER_SPARQL_CONNECTION_CLASS (klass)->init = tracker_sparql_connection_real_init;
    G_OBJECT_CLASS (klass)->finalize = tracker_sparql_connection_finalize;
}

/**
 * TrackerSparqlConnection:
 *
 * The <structname>TrackerSparqlConnection</structname> object represents a
 * connection with the Tracker store or databases depending on direct or
 * non-direct requests.
 */
GType tracker_sparql_connection_get_type (void) {
    static volatile gsize tracker_sparql_connection_type_id__volatile = 0;
    if (g_once_init_enter (&tracker_sparql_connection_type_id__volatile)) {
        static const GTypeInfo g_define_type_info = { sizeof (TrackerSparqlConnectionClass), (GBaseInitFunc) NULL, (GBaseFinalizeFunc) NULL, (GClassInitFunc) tracker_sparql_connection_class_init, (GClassFinalizeFunc) NULL, NULL, sizeof (TrackerSparqlConnection), 0, (GInstanceInitFunc) tracker_sparql_connection_instance_init, NULL };
        GType tracker_sparql_connection_type_id;
        tracker_sparql_connection_type_id = g_type_register_static (G_TYPE_OBJECT, "TrackerSparqlConnection", &g_define_type_info, G_TYPE_FLAG_ABSTRACT);
        g_once_init_leave (&tracker_sparql_connection_type_id__volatile, tracker_sparql_connection_type_id);
    }
    return tracker_sparql_connection_type_id__volatile;
}

static void parseResults(const QByteArray& buffer)
{
    QDomDocument doc(QLatin1String("sparqlresults"));
    if (!doc.setContent(buffer)) {
        return;
    }

    QDomElement sparqlElement = doc.documentElement();

    QDomNode sectionNode = sparqlElement.firstChild();
    while (!sectionNode.isNull()) {
        QDomElement sectionElement = sectionNode.toElement();
        if (!sectionElement.isNull()) {

            if (sectionElement.tagName() == QLatin1String("head")) {
                QDomNode variableNode = sectionElement.firstChild();
                while (!variableNode.isNull()) {
                    QDomElement variableElement = variableNode.toElement();
                    if (!variableElement.isNull()) {
                        variableNames.append(variableElement.text());
                    }

                    variableNode = variableNode.nextSibling();
                }
            } else if (sectionElement.tagName() == QLatin1String("boolean")) {
                // q->setBoolValue(sectionElement.text().toLower() == QLatin1String("true"));
            } else if (sectionElement.tagName() == QLatin1String("results")) {
                QDomNode resultsNode = sectionElement.firstChild();
                while (!resultsNode.isNull()) {
                    QDomElement resultsElement = resultsNode.toElement();
                    if (!resultsElement.isNull()) {
                        // QSparqlResultRow resultRow;
                        QDomNode resultNode = resultsElement.firstChild();

                        while (!resultNode.isNull()) {
                            QDomElement bindingElement = resultNode.toElement();

                            if (!bindingElement.isNull()) {
                                QDomNode bindingNode = bindingElement.firstChild();
                                QDomElement valueElement = bindingNode.toElement();

                                if (!valueElement.isNull()) {
                                    if (valueElement.tagName() == QLatin1String("bnode")) {
                                        variableValues.append(valueElement.text());
                                        variableDataTypes.append(TRACKER_SPARQL_VALUE_TYPE_BLANK_NODE);
                                    } else if (valueElement.tagName() == QLatin1String("literal")) {
                                        if (valueElement.hasAttribute(QString::fromLatin1("datatype"))) {
                                            if (valueElement.hasAttribute(QString::fromLatin1("xsi:type"))) {
                                                // No xsi:types in Tracker results
                                            } else {
                                                variableValues.append(valueElement.text());
                                                QByteArray s = valueElement.attribute(QString::fromLatin1("datatype")).toLatin1();
                                                if (s == "http://www.w3.org/2001/XMLSchema#integer") {
                                                    variableDataTypes.append(TRACKER_SPARQL_VALUE_TYPE_INTEGER);
                                                } else if (s == "http://www.w3.org/2001/XMLSchema#boolean") {
                                                    variableDataTypes.append(TRACKER_SPARQL_VALUE_TYPE_BOOLEAN);
                                                } else if (s == "http://www.w3.org/2001/XMLSchema#double") {
                                                    variableDataTypes.append(TRACKER_SPARQL_VALUE_TYPE_DOUBLE);
                                                } else if (s == "http://www.w3.org/2001/XMLSchema#string") {
                                                    variableDataTypes.append(TRACKER_SPARQL_VALUE_TYPE_STRING);
                                                } else if (s == "http://www.w3.org/2001/XMLSchema#dateTime") {
                                                    variableDataTypes.append(TRACKER_SPARQL_VALUE_TYPE_DATETIME);
                                                } else {
                                                    variableDataTypes.append(TRACKER_SPARQL_VALUE_TYPE_STRING);
                                                }
                                            }
                                        } else if (valueElement.hasAttribute(QString::fromLatin1("xml:lang"))) {
                                            // No xml:lang tags in Tracker results
                                        } else {
                                            variableValues.append(valueElement.text());
                                        }
                                    } else if (valueElement.tagName() == QLatin1String("uri")) {
                                        variableValues.append(valueElement.text());
                                        variableDataTypes.append(TRACKER_SPARQL_VALUE_TYPE_URI);
                                    } else {
                                        // Error
                                        qWarning() << "Unknown value element: " << valueElement.text();
                                    }
                                }
                            }

                            resultNode = resultNode.nextSibling();
                        }

                        (*async_cursor_next_callback)(0, async_cursor_result, async_cursor_next_user_data);
                    }

                    variableValues.clear();
                    variableDataTypes.clear();
                    resultsNode = resultsNode.nextSibling();
                }
            }
        }

        sectionNode = sectionNode.nextSibling();
    }

    (*async_cursor_next_callback)(0, async_cursor_result, async_cursor_next_user_data);
    return;
}

gboolean tracker_sparql_cursor_next_finish(TrackerSparqlCursor* self, GAsyncResult* _res_, GError** error)
{
    return (gboolean) variableValues.size() > 0;
}

gint tracker_sparql_cursor_get_n_columns(TrackerSparqlCursor* self)
{
    return (gint) variableNames.size();
}

const char* tracker_sparql_cursor_get_variable_name(TrackerSparqlCursor* self, gint column)
{
    return variableNames[(int) column].toLatin1().constData();
}

const char* tracker_sparql_cursor_get_string(TrackerSparqlCursor* self, gint column, glong* length)
{
    return variableValues[(int) column].toLatin1().constData();
}

TrackerSparqlValueType tracker_sparql_cursor_get_value_type(TrackerSparqlCursor* self, gint column)
{
    return variableDataTypes[(int) column];
}

void tracker_sparql_cursor_next_async(  TrackerSparqlCursor* self,
                                        GCancellable* cancellable,
                                        GAsyncReadyCallback callback,
                                        gpointer user_data )
{
    async_cursor_next_callback = callback;
    async_cursor_next_user_data = user_data;
}

TrackerSparqlCursor* tracker_sparql_connection_query_finish(TrackerSparqlConnection* self, GAsyncResult* _res_, GError** error)
{
    return (TrackerSparqlCursor*) g_object_new(TRACKER_SPARQL_TYPE_CURSOR, NULL);
}

void tracker_sparql_connection_update_finish(TrackerSparqlConnection* self, GAsyncResult* _res_, GError** error)
{
}

void tracker_sparql_connection_query_async( TrackerSparqlConnection* self,
                                            const char* sparql,
                                            GCancellable* cancellable,
                                            GAsyncReadyCallback callback,
                                            gpointer user_data )
{
    async_query_callback = callback;
    async_query_user_data = user_data;

    QString resultsFilename = QString::fromLatin1(getenv("QSPARQL_DUMMY_RESULTS"));
    QFile file(resultsFilename);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "QSPARQL_DUMMY_RESULTS environment variable: invalid file:" << resultsFilename;
        return;
    }

    parseResults(file.readAll());
    (*async_query_callback)(0, async_query_result, async_query_user_data);
}

void tracker_sparql_connection_update_async(    TrackerSparqlConnection* self,
                                                const char* sparql,
                                                gint priority,
                                                GCancellable* cancellable,
                                                GAsyncReadyCallback callback,
                                                gpointer user_data )
{
    async_update_callback = callback;
    async_update_user_data = user_data;
}

TrackerSparqlConnection* tracker_sparql_connection_get(GCancellable* cancellable, GError** error)
{
    return (TrackerSparqlConnection*) g_object_new(TRACKER_SPARQL_TYPE_CONNECTION, NULL);
}
