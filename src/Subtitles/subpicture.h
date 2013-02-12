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

#ifndef SUBPICTURE_H
#define SUBPICTURE_H

#include "erasepatch.h"

#include <QVector>

class SubPicture
{
public:
    SubPicture();
    SubPicture(const SubPicture& other);
    SubPicture(const SubPicture* other);
    virtual ~SubPicture() { }

    void setImageWidth(int w) { imageWidth = w; }
    void setImageHeight(int h) { imageHeight = h; }
    void setOfsX(int ofs) { xOfs = ofs; }
    void setOfsY(int ofs) { yOfs = ofs; }

    int width() { return screenWidth; }
    void setWidth(int width) { screenWidth = width; }
    int height() { return screenHeight; }
    void setHeight(int height) { screenHeight = height; }
    qint64 startTime() { return start; }
    void setStartTime(qint64 startTime) { start = startTime; }
    qint64 endTime() { return end; }
    void setEndTime(qint64 endTime) { end = endTime; }
    int compNum() { return compositionNumber; }
    void setCompNum(int compNum) { compositionNumber = compNum; }
    int numCompObjects() { return numberCompObjects; }
    void setNumCompObjects(int numCompObjects) { numberCompObjects = numCompObjects; }
    virtual bool isForced() { return forced; }
    virtual void setForced(bool isForced) { forced = isForced; }
    bool wasDecoded() { return decoded; }
    void setDecoded(bool wasDecoded) { decoded = wasDecoded; }
    bool exclude() { return excluded; }
    void setExclude(bool exclude) { excluded = exclude; }

    virtual int getImageWidth() { return imageWidth; }
    virtual int getImageHeight() {  return imageHeight; }
    virtual int getOfsX() { return xOfs; }
    virtual int getOfsY() { return yOfs; }

    SubPicture* copy();

    QVector<ErasePatch*> erasePatch;

private:
    int screenWidth = 0;
    int screenHeight = 0;
    qint64 start = -1;
    qint64 end = 0;
    int compositionNumber = 0;
    int numberCompObjects = 0;
    int imageWidth = 0;
    int imageHeight = 0;
    int xOfs = 0;
    int yOfs = 0;

    bool forced = false;
    bool decoded = false;
    bool excluded = false;
};

#endif // SUBPICTURE_H
