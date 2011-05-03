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

#include "trackerchangedsignalreceiver.h"
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusMetaType>
#include <QtDBus/QDBusArgument>
#include <QtCore/QTime>
#include <QtTest/QtTest>
#include <QtCore/QDebug>

Q_DECLARE_METATYPE(TrackerChangedSignalReceiver::Quad)
Q_DECLARE_METATYPE(QList<TrackerChangedSignalReceiver::Quad>)

namespace {
    const QLatin1String service("org.freedesktop.Tracker1");
    const QLatin1String resourcesInterface("org.freedesktop.Tracker1.Resources");
    const QLatin1String resourcesPath("/org/freedesktop/Tracker1/Resources");
    const QLatin1String changedSignal("GraphUpdated");
    const QLatin1String changedSignature("sa(iiii)a(iiii)");
}

TrackerChangedSignalReceiver::TrackerChangedSignalReceiver(QObject* parent)
    : QObject(parent), receivedSignals(0)
{
    qDBusRegisterMetaType<TrackerChangedSignalReceiver::Quad>();
    qDBusRegisterMetaType<QList<TrackerChangedSignalReceiver::Quad> >();
}

bool TrackerChangedSignalReceiver::connect(const QString& className)
{
    const bool connected = QDBusConnection::sessionBus().connect(
            service, resourcesPath,
          resourcesInterface, changedSignal,
          QStringList() << className,
          changedSignature,
          this, SLOT(changed(QString,QList<TrackerChangedSignalReceiver::Quad>,QList<TrackerChangedSignalReceiver::Quad>)));
    if (!connected)
        qWarning() << "TrackerChangedSignalReceiver::connect: could not connect to class" << className;
    return connected;
}

bool TrackerChangedSignalReceiver::disconnect(const QString& className)
{
    const bool disconnected = QDBusConnection::sessionBus().disconnect(
            service, resourcesPath,
          resourcesInterface, changedSignal,
          QStringList() << className,
          changedSignature,
          this, SLOT(changed(QString,QList<TrackerChangedSignalReceiver::Quad>,QList<TrackerChangedSignalReceiver::Quad>)));
    if (!disconnected)
        qWarning() << "TrackerChangedSignalReceiver::disconnect: could not disconnect from class" << className;
    return disconnected;
}

void TrackerChangedSignalReceiver::drain(int forMs)
{
    int lastSignalCount = receivedSignals;
    QTime timer;
    timer.start();
    bool waiting = true;
    while ((waiting = timer.elapsed() < forMs)) {
        QTest::qWait(100);
        if (receivedSignals > lastSignalCount) {
            timer.restart();
            lastSignalCount = receivedSignals;
        }
    }
}

void TrackerChangedSignalReceiver::clear()
{
    receivedSignals = 0;
    receivedDeleted.clear();
    receivedInserted.clear();
}

void TrackerChangedSignalReceiver::changed(QString,
             QList<TrackerChangedSignalReceiver::Quad> deleted,
             QList<TrackerChangedSignalReceiver::Quad> inserted)
{
    ++receivedSignals;
    receivedDeleted.append(deleted);
    receivedInserted.append(inserted);
}

// D-Bus marshalling
QDBusArgument& operator<<(QDBusArgument& argument,
                          const TrackerChangedSignalReceiver::Quad& t)
{
    argument.beginStructure();
    argument << t.graph << t.subject << t.predicate << t.object;
    argument.endStructure();
    return argument;
}

// D-Bus demarshalling
const QDBusArgument& operator>>(const QDBusArgument& argument,
                                TrackerChangedSignalReceiver::Quad& t)
{
    argument.beginStructure();
    argument >> t.graph >> t.subject >> t.predicate >> t.object;
    argument.endStructure();
    return argument;
}
