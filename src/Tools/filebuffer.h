/*
 * BDSup2Sub++ (C) 2012 Adam T.
 * Based on code from BDSup2Sub by Copyright 2009 Volker Oth (0xdeadbeef)
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

    int getDWord(long ofs);
    int getByte(long ofs);
    int getWord(long ofs);
    void getBytes(long ofs, QVector<uchar> &b, int length);
     long getSize() { return length; }
    int getDWordLE(long ofs);

private:
    void readBuffer(long ofs);

    QByteArray buf;
    QString fileName;
    QScopedPointer<QFile> file;
    long offset = 0;
    long offsetEnd = 0;
    long length = 0;
};

#endif // FILEBUFFER_H
