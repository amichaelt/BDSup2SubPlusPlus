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

class FileBuffer
{
public:
    FileBuffer(QString inFileName);
    ~FileBuffer()
    {
        if (!file.isNull())
        {
            file.reset();
        }
    }

    int getDWord(uint ofs)
    {
        return (buf[ofs + 3] & 0xff) | ((buf[ofs + 2] & 0xff) << 8) |
              ((buf[ofs + 1] & 0xff) <<16) | ((buf[ofs] & 0xff) << 24);
    }
    int getByte(uint ofs) { return (buf[ofs] & 0xff); }
    int getWord(uint ofs) { return (buf[ofs + 1] & 0xff) | ((buf[ofs] & 0xff) << 8); }
    void getBytes(uint ofs, uchar *b, int length)
    {
        memcpy(b, (buf.data() + ofs), length);
    }
    int getDWordLE(uint ofs)
    {
        return (buf[ofs] & 0xff) | ((buf[ofs + 1] & 0xff) << 8)
                | ((buf[ofs + 2] & 0xff) << 16) | ((buf[ofs + 3] & 0xff) << 24);
    }

    qint64 getSize() { return length; }

private:
    qint64 offset = 0;
    qint64 offsetEnd = 0;
    qint64 length = 0;

    QByteArray buf;

    QScopedPointer<QFile> file;

    QString fileName;

    void readBuffer(qint64 ofs);
};

#endif // FILEBUFFER_H
