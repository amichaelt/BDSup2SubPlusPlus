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

#include "filebuffer.h"

#include <QDataStream>

FileBuffer::FileBuffer(QString inFileName) :
    fileName(inFileName)
{
    file.reset(new QFile(fileName));
    length = file->size();
    if (!file->open(QIODevice::ReadOnly))
    {
        throw QString("File: '%1' can not be opened for reading.").arg(fileName);
    }

    buf.resize(length);
    file->read(buf.data(), length);
    if (buf.isEmpty() && file->error() != QFile::NoError)
    {
        throw QString("IO error at offset +%1 of file: '%2'")
                .arg(QString::number(0)).arg(fileName);
    }
}
