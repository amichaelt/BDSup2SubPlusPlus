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
    SubPictureDVD(const SubPictureDVD &other);
    SubPictureDVD(const SubPictureDVD *other);
    ~SubPictureDVD() { }

    void setOriginal();
    void copyInfo(SubPicture &subPicture);

    int offset() { return pictureOffset; }
    void setOffset(int offset) { pictureOffset = offset; }
    int rleSize() { return size; }
    void setRleSize(int rleSize) { size = rleSize; }
    int evenOffset() { return evenOfs; }
    void setEvenOffset(int evenOffset) { evenOfs = evenOffset; }
    int oddOffset() { return oddOfs; }
    void setOddOffset(int oddOffset) { oddOfs = oddOffset; }
    int originalWidth() { return origWidth; }
    void setOriginalWidth(int originalWidth) { origWidth = originalWidth; }
    int originalHeight() { return origHeight; }
    void setOriginalHeight(int originalHeight) { origHeight = originalHeight; }
    int originalX() { return origX; }
    void setOriginalX(int originalX) { origX = originalX; }
    int originalY() { return origY; }
    void setOriginalY(int originalY) { origY = originalY; }

private:
    qint64 pictureOffset = 0;
    int size = 0;
    int evenOfs = 0;
    int oddOfs = 0;
    int origWidth = 0;
    int origHeight = 0;
    int origX = 0;
    int origY = 0;

public:
    QVector<ImageObjectFragment> rleFragments;
    QVector<int> originalAlpha = QVector<int>(4);
    QVector<int> originalPal = QVector<int>(4);
    QVector<int> alpha = QVector<int>(4);
    QVector<int> pal = QVector<int>(4);
};

#endif // SUBPICTUREDVD_H
