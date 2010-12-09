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

#ifndef _GALLERY_BENCHMARK_H
#define _GALLERY_BENCHMARK_H

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtTest>
#include <QtSparql>

class GalleryBenchmark : public QObject
{
        Q_OBJECT
public:
        GalleryBenchmark();

public slots:
        void initTestCase();

private slots:
        void test_gallery_01();
        void test_gallery_02();
        void test_gallery_03();
        void test_gallery_04();
        void test_gallery_05();
        void test_gallery_06();
        void test_gallery_07();
        void test_gallery_08();
        void test_gallery_09();
        void test_gallery_10();
        void test_gallery_11();
        void test_gallery_12();
        void test_gallery_13();
};

enum SortOrder {
    Date = 0,
    Name = 1
};

#endif /* _GALLERY_BENCHMARK_H */
