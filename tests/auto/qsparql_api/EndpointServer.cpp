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

#include "EndpointServer.h"
#include <QTextStream>
#include <QTcpSocket>
#include <QStringList>
#include <QUrl>
#include <QDomDocument>
#include <QtSparql/QtSparql>

EndpointServer::EndpointServer(int _port) : port(_port), disabled(true)
{
    if (!listen(QHostAddress::Any, port)) {
        qWarning() << "Can't bind server to port "<< port;
    } else {
        disabled=false;
        //qDebug() << "Starting fake endpoint server";
    }
}

EndpointServer::~EndpointServer()
{
    stop();
}

void EndpointServer::stop()
{
    disabled=true;
    //qDebug() << "Shutting down fake endpoint www server";
    close();
}

void EndpointServer::incomingConnection(int socket)
{
    if (disabled)
        return;

    QTcpSocket* s = new QTcpSocket(this);
    connect(s, SIGNAL(readyRead()), this, SLOT(readClient()));
    connect(s, SIGNAL(disconnected()), this, SLOT(discardClient()));
    s->setSocketDescriptor(socket);
}

QString EndpointServer::sparqlData(QString url)
{
    QStringList urlSplitted=url.split("query=");
    QUrl urli = QUrl::fromEncoded(urlSplitted.at(1).toAscii());
    QString query = urli.toString();

    // returned data is based on http://www.w3.org/TR/rdf-sparql-protocol/
    QString result;
    result="HTTP/1.0 200 Ok\r\n"
           "Content-Type: text/html; charset=\"utf-8\"\r\n"
           "\r\n";

    //basic XML tree
    QDomDocument tracker_result;
    QDomElement sparql = tracker_result.createElement("sparql");
    QDomElement results = tracker_result.createElement("results");
    sparql.setAttribute("xmlns", "http://www.w3.org/2005/sparql-results#");


    QSparqlConnection conn("QTRACKER_DIRECT");
    QSparqlQuery::StatementType queryType = QSparqlQuery::SelectStatement;
    if(query.contains("insert"))
        queryType = QSparqlQuery::InsertStatement;
    else if(query.contains("delete"))
        queryType = QSparqlQuery::DeleteStatement;
    else if(query.contains("ask"))
        queryType = QSparqlQuery::AskStatement;
    QSparqlQuery q(query, queryType);
    QSparqlResult* r = conn.exec(q);
    r->waitForFinished();

    if(r->hasError()) {
        return result + "error";
    }
    else if (r->isBool()) {
        QDomElement boolean = tracker_result.createElement("boolean");
        QDomText value = tracker_result.createTextNode(r->boolValue()?"true":"false");
        boolean.appendChild(value);
        sparql.appendChild(boolean);
    }
    else
    {
        while (r->next()) {
            QDomElement result = tracker_result.createElement("result");
            int index=0;
            while(r->binding(index).isValid())
            {
                QDomElement binding = tracker_result.createElement("binding");
                QDomText value = tracker_result.createTextNode(r->binding(index).value().toString());
                if(r->binding(index).isUri())
                {
                    QDomElement uri = tracker_result.createElement("uri");
                    uri.appendChild(value);
                    binding.appendChild(uri);
                }
                if(r->binding(index).isLiteral())
                {
                    QDomElement literal = tracker_result.createElement("literal");
                    literal.appendChild(value);
                    binding.appendChild(literal);
                }

                result.appendChild(binding);
                index++;
            }
            results.appendChild(result);
        }
        sparql.appendChild(results);
    }
    //append xml to html header
    tracker_result.appendChild(sparql);
    result+= tracker_result.toString();
    return result;
}

void EndpointServer::readClient()
{
    if (disabled)
        return;

    // This slot is called when the client sent data to the server. The
    // server looks if it was a get request and sends a very simple HTML
    // document back.
    QTcpSocket* socket = (QTcpSocket*)sender();
    if (socket->canReadLine()) {
        QStringList tokens = QString(socket->readLine()).split(QRegExp("[ \r\n][ \r\n]*"));
        if (tokens[0] == "GET") {
            QString url = tokens[1];
            QTextStream os(socket);
            os.setAutoDetectUnicode(true);
            os << sparqlData(url);
            socket->close();

            if (socket->state() == QTcpSocket::UnconnectedState) {
                delete socket;
            }
        }
    }
}

void EndpointServer::discardClient()
{
    QTcpSocket* socket = (QTcpSocket*)sender();
    socket->deleteLater();
}

bool EndpointServer::isRunning() const
{
    return !disabled;
}

void EndpointServer::pause()
{
    disabled=true;
}

bool EndpointServer::resume()
{
    if (isListening()) {
        disabled=false;
        return true;
    } else {
        //qDebug() << "Can't resume server as there was problem with binding on port " << port;
        return false;
    }
}
