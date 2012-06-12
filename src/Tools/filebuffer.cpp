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

#include "filebuffer.h"

#include <QDataStream>

FileBuffer::FileBuffer(QString inFileName) :
    fileName(inFileName),
    file(fileName)
{
    length = file.size();
    if (!file.open(QIODevice::ReadOnly))
    {
        //TODO: error handling
        throw QString("File '%1' not found").arg(fileName);
    }
    readBuffer(offset);
}

int FileBuffer::getDWord(long ofs)
{
    if ((ofs < offset) || ((ofs + 3) > offsetEnd))
    {
        readBuffer(ofs);
    }
    int idx = (int)(ofs - offset);
    return (buf[idx + 3] & 0xff) | ((buf[idx + 2] & 0xff) << 8) |
          ((buf[idx + 1] & 0xff) <<16) | ((buf[idx] & 0xff) << 24);
}

int FileBuffer::getByte(long ofs)
{
    if ((ofs < offset) || (ofs > offsetEnd))
    {
        readBuffer(ofs);
    }
    return (buf[(int)(ofs - offset)] & 0xff);
}

int FileBuffer::getWord(long ofs)
{
    if ((ofs < offset) || ((ofs + 1) > offsetEnd))
    {
        readBuffer(ofs);
    }
    int idx = (int)(ofs - offset);
    return (buf[idx + 1] & 0xff) | ((buf[idx] & 0xff) << 8);
}

void FileBuffer::getBytes(long ofs, QVector<uchar>& b, int length)
{
    if (ofs < offset || ((ofs + length) - 1) > offsetEnd)
    {
        readBuffer(ofs);
    }
    for (int i = 0; i < length; ++i)
    {
        b.replace(i, buf[(int)((ofs - offset) + i)]);
    }
}

int FileBuffer::getDWordLE(long ofs)
{
    if (ofs < offset || (ofs + 3) > offsetEnd)
    {
        readBuffer(ofs);
    }
    int idx = (int)(ofs - offset);
    return (buf.at(idx) & 0xff) | ((buf.at(idx + 1) & 0xff) << 8)
            | ((buf.at(idx + 2) & 0xff) << 16) | ((buf.at(idx + 3) & 0xff) << 24);
}

void FileBuffer::close()
{
    if (file.isOpen())
    {
        file.close();
    }
}

void FileBuffer::readBuffer(long ofs)
{
    if (file.isOpen())
    {
        offset = ofs;
        file.seek(offset);
        long l = length - offset;
        if (l < 0)
        {
            close();
            throw QString("Offset %1 out of bounds for file '%2'")
                    .arg(QString::number(ofs)).arg(fileName);
        }
        buf = file.read(l);
        if (buf.isEmpty() && file.error() != QFile::NoError)
        {
            close();
            throw QString("IO error at offset +%1 of file '%2'")
                    .arg(QString::number(ofs)).arg(fileName);
        }
        offsetEnd = (offset + buf.size()) - 1;
    }
}
