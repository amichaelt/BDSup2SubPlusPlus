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

#ifndef SUBSTREAM_H
#define SUBSTREAM_H

#include <QVector>
#include <QSharedData>

class Palette;
class QImage;
class Bitmap;
class SubPicture;
class SubPictureDVD;
class FileBuffer;
class SubtitleProcessor;

class Substream : public QSharedData
{
public:
    virtual Palette *getPalette() = 0;
    virtual Bitmap *getBitmap() = 0;
    virtual QImage *getImage() = 0;
    virtual QImage *getImage(Bitmap *bitmap) = 0;
    virtual int getPrimaryColorIndex() = 0;
    virtual void decode(int index) = 0;
    virtual int getNumFrames() = 0;
    virtual int getNumForcedFrames() = 0;
    virtual bool isForced(int index) = 0;
    virtual long getEndTime(int index) = 0;
    virtual long getStartTime(int index) = 0;
    virtual long getStartOffset(int index) = 0;
    virtual SubPicture *getSubPicture(int index) = 0;

protected:
    int numForcedFrames = 0;
};

#endif // SUBSTREAM_H
