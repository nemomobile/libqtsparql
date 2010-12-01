/*
 * Copyright (C) 2009 Nokia Corporation.
 *
 * Contact: Marius Vollmer <marius.vollmer@nokia.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */
#ifndef _MUSIC_BENCHMARK_H
#define _MUSIC_BENCHMARK_H

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtTest>
#include <QtSparql>

class MusicBenchmark : public QObject
{
        Q_OBJECT
public:
        MusicBenchmark();
        void initTestCase();

private slots:
        void test_audio_01();
        void test_audio_02();
        void test_audio_03();
        void test_audio_04();
        void test_audio_05();
        void test_audio_06();
        void test_audio_07();
        void test_audio_08();
        void test_audio_09();
        void test_audio_10();
        void test_audio_11();
};

#endif /* _MUSIC_BENCHMARK_H */
