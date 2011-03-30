/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (ivan.frade@nokia.com)
**
** This file is part of the examples of the QtSparql module (not yet part of the Qt Toolkit).
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
#include "sparqlquerytext.h"

#include <QtSparql/QSparqlResultRow>
#include <QtSparql/QSparqlError>

#include <QtGui/QHeaderView>

SparqlQueryText::SparqlQueryText(QSparqlConnection& conn, QWidget *parent)
    : QTextEdit(parent), connection(conn)
{
    // connection.addPrefix("foaf", QUrl::fromEncoded("http://xmlns.com/foaf/0.1/"));
    // setText(QLatin1String("SELECT ?Predicate ?Object \nWHERE { <http://dbpedia.org/resource/The_Beatles> ?Predicate ?Object . }"));
    connection.addPrefix("sch-ont", QUrl::fromEncoded("http://education.data.gov.uk/def/school/"));
    setText(QLatin1String("SELECT ?name WHERE { "
                                               "?school a sch-ont:School; "
                                               "sch-ont:establishmentName ?name; "
                                               "sch-ont:districtAdministrative <http://statistics.data.gov.uk/id/local-authority-district/00AA> ; "
                                               "} "
                                               "ORDER BY ?name"));
    model = new QSparqlQueryModel();

    connect(model, SIGNAL(finished()), this, SLOT(queryFinished()));
    tableView = new QTableView();
    tableView->setModel(model);
    tableView->resize(1000, 600);
    tableView->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
}

void SparqlQueryText::runQuery()
{
    query = new QSparqlQuery(toPlainText());
    model->setQuery(*query, connection);
    tableView->show();
}

void SparqlQueryText::queryFinished()
{
    qDebug() << "Error message:" << model->lastError().message();
}



