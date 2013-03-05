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
#include <QRect>

class ImageObject;
class PaletteInfo;

class SubPictureBD : public SubPicture
{
public:
    SubPictureBD();
    SubPictureBD(const SubPictureBD* other);
    SubPictureBD(const SubPictureBD& other);
    ~SubPictureBD() { }

    SubPicture* copy();

    void setX(int ofs, bool isMoving = false)
    {
        if (isMoving)
        {
            int diff = ofs - xOfs;
            for (int i = 0; i < WindowSizes.size(); ++i)
            {
                if (WindowSizes[i].x() == ofs)
                {
                    continue;
                }
                WindowSizes[i].setX(WindowSizes[i].x() + diff);
            }
        }
        xOfs = ofs;
    }

    void setY(int ofs, bool isMoving = false)
    {
        if (isMoving)
        {
            int diff = ofs - yOfs;
            for (int i = 0; i < WindowSizes.size(); ++i)
            {
                if (WindowSizes[i].y() == ofs)
                {
                    continue;
                }
                WindowSizes[i].setY(WindowSizes[i].y() + diff);
            }
        }
        yOfs = ofs;
    }

    virtual void setImageWidth(int w, bool isScaling = false)
    {
        if (isScaling)
        {
            for (int i = 0; i < WindowSizes.size(); ++i)
            {
                double scaleFactor = (double) w / imageObjectList[i].width();
                WindowSizes[i].setWidth(WindowSizes[i].width() * scaleFactor);
            }
        }
        _imageWidth = w;
    }

    virtual void setImageHeight(int h, bool isScaling = false)
    {
        if (isScaling)
        {
            for (int i = 0; i < WindowSizes.size(); ++i)
            {
                double scaleFactor = (double) h / imageObjectList[i].height();
                WindowSizes[i].setHeight(WindowSizes[i].height() * scaleFactor);
            }
        }
        _imageHeight = h;
    }

    bool paletteUpdated() { return paletteUpdate; }
    void setPaletteUpdated(bool paletteUpdated) { paletteUpdate = paletteUpdated; }
    int numberOfWindows() { return numWindows; }
    void setNumberOfWindows(int numberOfWindows) { numWindows = numberOfWindows; }
    int subPictureType() { return type; }
    void setSubPictureType(int subPictureType) { type = subPictureType; }

    bool isForced()
    {
        bool isForced = true;

        for (int i = 0; i < imageObjectList.size(); ++i)
        {
            if (imageObjectList[i].fragmentList().size() > 0)
            {
                isForced &= imageObjectList[i].isForced();
            }
        }
        return isForced;
    }

    void setForced(bool isForced)
    {
        for (int i = 0; i < imageObjectList.size(); ++i)
        {
            if (imageObjectList[i].fragmentList().size() > 0)
            {
                imageObjectList[i].setForced(isForced);
            }
        }
    }

    QVector<ImageObject> imageObjectList;

    QVector<QVector<PaletteInfo>> palettes;

    QVector<QRect> WindowSizes;

    ImageObject &getImgObj(int index) { return imageObjectList[index]; }

private:
    int type = 0;
    int numWindows = 0;
    bool paletteUpdate;
};

#endif // SUBSPICTUREBD_H
