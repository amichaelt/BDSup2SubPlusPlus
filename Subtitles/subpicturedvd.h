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

    long offset;
    int rleSize;
    int evenOfs;
    int oddOfs;
    QVector<ImageObjectFragment> rleFragments;
    int originalWidth;
    int originalHeight;
    int originalX;
    int originalY;
    QVector<int> originalAlpha;
    QVector<int> originalPal;
    QVector<int> alpha;
    QVector<int> pal;
};

#endif // SUBPICTUREDVD_H
