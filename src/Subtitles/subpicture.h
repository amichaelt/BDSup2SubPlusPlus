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

#ifndef SUBPICTURE_H
#define SUBPICTURE_H

#include "erasepatch.h"

#include <QVector>

class SubPicture
{
public:
    SubPicture();
    SubPicture(const SubPicture& other);

    SubPicture* copy();
    virtual int getImageWidth() { return imageWidth; }
    virtual int getImageHeight() {  return imageHeight; }
    virtual int getOfsX() { return xOfs; }
    virtual int getOfsY() { return yOfs; }
    void setImageWidth(int w) { imageWidth = w; }
    void setImageHeight(int h) { imageHeight = h; }
    void setOfsX(int ofs) { xOfs = ofs; }
    void setOfsY(int ofs) { yOfs = ofs; }

    int width;
    int height;
    long startTime;
    long endTime;
    bool isForced;
    int compNum;
    bool wasDecoded;
    bool exclude;
    QVector<ErasePatch*> erasePatch;

private:
    int imageWidth;
    int imageHeight;
    int xOfs;
    int yOfs;
};

#endif // SUBPICTURE_H
