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

#ifndef TRACKERCHANGEDSIGNALRECEIVER_H
#define TRACKERCHANGEDSIGNALRECEIVER_H

#include <QtCore/QObject>
#include <QtCore/QList>

class TrackerChangedSignalReceiver : public QObject
{
    Q_OBJECT

public:
    struct Quad
    {
        int graph, subject, predicate, object;
    };

    TrackerChangedSignalReceiver(QObject* parent=0);
    bool connect(const QString& className);
    bool disconnect(const QString& className);
    void drain(int forMs);
    void clear();

    int receivedSignals;
    QList<Quad> receivedDeleted;
    QList<Quad> receivedInserted;

private slots:
    void changed(QString, QList<TrackerChangedSignalReceiver::Quad> deleted, QList<TrackerChangedSignalReceiver::Quad> inserted);
};

#endif // TRACKERCHANGEDSIGNALRECEIVER_H
