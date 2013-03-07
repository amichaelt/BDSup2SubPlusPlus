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

    virtual void setImageWidth(int w, double scaleFactor = 1)
    {
        if (scaleFactor != 1.0d)
        {
            for (int i = 0; i < scaledWindows.size(); ++i)
            {
                scaledWindows[i].setWidth((int)((scaledWindows[i].width() * scaleFactor) + 0.5));
            }
        }
        else
        {
            scaledWindows = windows;
        }
        _imageWidth = w;
    }

    virtual void setImageHeight(int h, double scaleFactor = 1)
    {
        if (scaleFactor != 1.0d)
        {
            for (int i = 0; i < scaledWindows.size(); ++i)
            {
                scaledWindows[i].setHeight((int)((scaledWindows[i].height() * scaleFactor) + 0.5));
            }
        }
        else
        {
            scaledWindows = windows;
        }
        _imageHeight = h;
    }

    virtual void setX(int ofs, double scaleFactor = 1)
    {
        if (scaleFactor != 1.0d)
        {
            for (int i = 0; i < scaledWindows.size(); ++i)
            {
                scaledWindows[i].setX((int)((scaledWindows[i].x() * scaleFactor) + 0.5));
            }
        }
        else
        {
            scaledWindows = windows;
        }
        xOfs = ofs;
    }

    virtual void setY(int ofs, double scaleFactor = 1)
    {
        if (scaleFactor != 1.0d)
        {
            for (int i = 0; i < scaledWindows.size(); ++i)
            {
                scaledWindows[i].setY((int)((scaledWindows[i].y() * scaleFactor) + 0.5));
            }
        }
        else
        {
            scaledWindows = windows;
        }
        yOfs = ofs;
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

    QVector<QRect> &windowSizes() { return scaledWindows; }
    void setWindowSizes(QVector<QRect> windowSizes) { windows = scaledWindows = windowSizes; }


    ImageObject &getImgObj(int index) { return imageObjectList[index]; }

private:
    int type = 0;
    int numWindows = 0;
    bool paletteUpdate;

    QVector<QRect> scaledWindows;
    QVector<QRect> windows;
};

#endif // SUBSPICTUREBD_H
