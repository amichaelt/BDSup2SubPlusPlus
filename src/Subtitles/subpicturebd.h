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

#ifndef SUBPICTUREBD_H
#define SUBPICTUREBD_H

#include "subpicture.h"
#include "imageobject.h"
#include "paletteinfo.h"

#include <QVector>

class ImageObject;
class PaletteInfo;

class SubPictureBD : public SubPicture
{
public:
    SubPictureBD();
    SubPictureBD(const SubPictureBD* other);
    SubPictureBD(const SubPictureBD& other);
    ~SubPictureBD() { }

    int objectID() { return objectId; }
    void setObjectID(int objectID) { objectId = objectID; }
    int windowWidth() { return winWidth; }
    void setWindowWidth(int windowWidth) { winWidth = windowWidth; }
    int windowHeight() { return winHeight; }
    void setWindowHeight(int windowHeight) { winHeight = windowHeight; }
    int xWindowOffset() { return xWinOfs; }
    void setXWindowOffset(int xWindowOffset) { xWinOfs = xWindowOffset; }
    int yWindowOffset() { return yWinOfs; }
    void setYWindowOffset(int yWindowOffset) { yWinOfs = yWindowOffset; }
    int subPictureType() { return type; }
    void setSubPictureType(int subPictureType) { type = subPictureType; }

    QVector<ImageObject> imageObjectList;

    QVector<QVector<PaletteInfo> > palettes;

    ImageObject &getImgObj(int index) { return imageObjectList[index]; }
    ImageObject &getImgObj() { return imageObjectList[objectId]; }

private:
    int objectId = 0;
    int winWidth = 0;
    int winHeight = 0;
    int xWinOfs = 0;
    int yWinOfs = 0;
    int type = 0;
};

#endif // SUBSPICTUREBD_H
