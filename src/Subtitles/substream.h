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

#ifndef SUBSTREAM_H
#define SUBSTREAM_H

#include <QtGlobal>

class Bitmap;
class FileBuffer;
class Palette;
class QImage;
class SubPicture;
class SubPictureDVD;
class SubtitleProcessor;

class Substream
{
public:
    virtual ~Substream() { }

    virtual void decode(int index) = 0;

    virtual int primaryColorIndex() = 0;
    virtual int numFrames() = 0;
    virtual int numForcedFrames() = 0;

    virtual qint64 endTime(int index) = 0;
    virtual qint64 startTime(int index) = 0;
    virtual qint64 startOffset(int index) = 0;

    virtual bool isForced(int index) = 0;

    virtual Bitmap &bitmap() = 0;

    virtual Palette &palette() = 0;

    virtual SubPicture *subPicture(int index) = 0;

    virtual QImage image() = 0;
    virtual QImage image(Bitmap &bitmap) = 0;

protected:
    int _numForcedFrames = 0;
};

#endif // SUBSTREAM_H
