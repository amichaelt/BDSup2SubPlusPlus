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

#ifndef SUBPICTUREDVD_H
#define SUBPICTUREDVD_H

#include "subpicture.h"
#include "imageobjectfragment.h"

#include <QVector>

class SubPictureDVD : public SubPicture
{
public:
    SubPictureDVD();

    void setOriginal();
    void copyInfo(SubPicture* subPicture);

    long offset = 0;
    int rleSize = 0;
    int evenOfs = 0;
    int oddOfs = 0;
    QVector<ImageObjectFragment*> rleFragments = QVector<ImageObjectFragment*>();
    int originalWidth = 0;
    int originalHeight = 0;
    int originalX = 0;
    int originalY = 0;
    QVector<int> originalAlpha = QVector<int>(4);
    QVector<int> originalPal = QVector<int>(4);
    QVector<int> alpha = QVector<int>(4);
    QVector<int> pal = QVector<int>(4);
};

#endif // SUBPICTUREDVD_H
