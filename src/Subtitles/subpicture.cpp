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

#include "subpicture.h"

SubPicture::SubPicture() :
    width(0),
    height(0),
    startTime(0),
    endTime(0),
    isForced(false),
    compNum(0),
    wasDecoded(false),
    exclude(false),
    erasePatch(),
    imageWidth(0),
    imageHeight(0),
    xOfs(0),
    yOfs(0)
{
}

SubPicture::SubPicture(const SubPicture &other) :
    width(other.width),
    height(other.height),
    startTime(other.startTime),
    endTime(other.endTime),
    isForced(other.isForced),
    compNum(other.compNum),
    wasDecoded(other.wasDecoded),
    exclude(other.exclude),
    erasePatch(other.erasePatch),
    imageWidth(other.imageWidth),
    imageHeight(other.imageHeight),
    xOfs(other.xOfs),
    yOfs(other.yOfs)
{
}

SubPicture::SubPicture(const SubPicture *other) :
    width(other->width),
    height(other->height),
    startTime(other->startTime),
    endTime(other->endTime),
    isForced(other->isForced),
    compNum(other->compNum),
    wasDecoded(other->wasDecoded),
    exclude(other->exclude),
    erasePatch(other->erasePatch),
    imageWidth(other->imageWidth),
    imageHeight(other->imageHeight),
    xOfs(other->xOfs),
    yOfs(other->yOfs)
{
}

SubPicture* SubPicture::copy()
{
    SubPicture* sp = new SubPicture;
    sp->width = width;
    sp->height = height;
    sp->startTime = startTime;
    sp->endTime = endTime;
    sp->isForced = isForced;
    sp->compNum = compNum;
    sp->setImageWidth(getImageWidth());
    sp->setImageHeight(getImageHeight());
    sp->setOfsX(getOfsX());
    sp->setOfsY(getOfsY());
    sp->exclude = exclude;
    sp->wasDecoded = wasDecoded;
    if (!erasePatch.empty())
    {
        sp->erasePatch = erasePatch;
    }

    return sp;
}
