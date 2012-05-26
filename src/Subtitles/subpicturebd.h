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

#ifndef SUBPICTUREBD_H
#define SUBPICTUREBD_H

#include "subpicture.h"
#include "imageobjectfragment.h"
#include "paletteinfo.h"
#include <QVector>

class ImageObject;
class PaletteInfo;

class SubPictureBD : public SubPicture
{
public:
    SubPictureBD();
    SubPictureBD(SubPictureBD* other);

    int objectID = 0;
    QVector<ImageObject*> imageObjectList = QVector<ImageObject*>();
    int winWidth = 0;
    int winHeight = 0;
    int xWinOfs = 0;
    int yWinOfs = 0;
    int type = 0;
    QVector<QVector<PaletteInfo*>*> palettes = QVector<QVector<PaletteInfo*>*>();

    ImageObject* getImgObj(int index) { return imageObjectList.at(index); }
    ImageObject* getImgObj() { return imageObjectList.at(objectID); }
    int getImageWidth();
    int getImageHeight();
    int getOfsX();
    int getOfsY();
};

#endif // SUBSPICTUREBD_H
