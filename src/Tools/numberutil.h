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

#ifndef NUMBERUTIL_H
#define NUMBERUTIL_H

#include <QString>
#include <QVector>

class NumberUtil
{
public:
    static void setByte(QVector<uchar>& buf, int index, int value)
    {
        buf.replace(index, (uchar)value);
    }
    static void setWord(QVector<uchar>& buf, int index, int value)
    {
        buf.replace(index, (uchar)(value >> 8));
        buf.replace(index + 1, (uchar)value);
    }
    static void setDWord(QVector<uchar>& buf, int index, int value)
    {
        buf.replace(index, (uchar)(value >> 24));
        buf.replace(index + 1, (uchar)(value >> 16));
        buf.replace(index + 2, (uchar)(value >> 8));
        buf.replace(index + 3, (uchar)value);
    }
    static void setString(QVector<uchar> &buf, int index, QString string);
};

#endif // NUMBERUTIL_H
