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

#ifndef IMAGEOBJECT_H
#define IMAGEOBJECT_H

#include "imageobjectfragment.h"

#include <QVector>

class ImageObject
{
public:
    ImageObject();
    ImageObject(const ImageObject* other);
    ImageObject(const ImageObject& other);
    ~ImageObject() { fragmentList.clear(); }

    int paletteID() { return paletteId; }
    void setPaletteID(int id) { paletteId = id; }
    int bufferSize() { return bufSize; }
    void setBufferSize(int size) { bufSize = size; }
    int width() { return w; }
    void setWidth(int width) { w = width; }
    int height() { return h; }
    void setHeight(int height) { h = height; }
    int xOffset() { return xOfs; }
    void setXOffset(int offset) { xOfs = offset; }
    int yOffset() { return yOfs; }
    void setYOffset(int offset) { yOfs = offset; }

    QVector<ImageObjectFragment> &getFragmentList() { return fragmentList; }

private:
    int paletteId = 0;
    int bufSize = 0;
    int w = 0;
    int h = 0;
    int xOfs = 0;
    int yOfs = 0;

    QVector<ImageObjectFragment> fragmentList;
};

#endif // IMAGEOBJECT_H
