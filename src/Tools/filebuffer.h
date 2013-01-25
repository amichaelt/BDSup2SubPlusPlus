/*
 * BDSup2Sub++ (C) 2012 Adam T.
 * Based on code from BDSup2Sub by Copyright 2009 Volker Oth (0xdeadbeef)
 * and Copyright 2012 Miklos Juhasz (mjuhasz)
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef FILEBUFFER_H
#define FILEBUFFER_H

#include <QByteArray>
#include <QFile>
#include <QFileInfo>
#include <QString>
#include <QVector>

static const int BUFFERSIZE = 1024*1024;

class FileBuffer
{
public:
    FileBuffer(QString inFileName);
    ~FileBuffer();

    int getDWord(quint64 ofs);
    int getByte(quint64 ofs);
    int getWord(quint64 ofs);
    void getBytes(quint64 ofs, QVector<uchar> &b, int length);
     quint64 getSize() { return length; }
    int getDWordLE(quint64 ofs);

private:
    quint64 offset = 0;
    quint64 offsetEnd = 0;
    quint64 length = 0;

    QByteArray buf;

    QScopedPointer<QFile> file;

    QString fileName;

    void readBuffer(quint64 ofs);
};

#endif // FILEBUFFER_H
