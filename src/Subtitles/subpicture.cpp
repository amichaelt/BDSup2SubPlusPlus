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

#include "subpicture.h"

SubPicture::SubPicture()
{
}

SubPicture::SubPicture(const SubPicture &other) :
    erasePatch(other.erasePatch),
    screenWidth(other.screenWidth),
    screenHeight(other.screenHeight),
    start(other.start),
    end(other.end),
    compositionNumber(other.compositionNumber),
    numberCompObjects(other.numberCompObjects),
    imageWidth(other.imageWidth),
    imageHeight(other.imageHeight),
    xOfs(other.xOfs),
    yOfs(other.yOfs),
    forced(other.forced),
    decoded(other.decoded),
    excluded(other.excluded)
{
}

SubPicture::SubPicture(const SubPicture *other) :
    erasePatch(other->erasePatch),
    screenWidth(other->screenWidth),
    screenHeight(other->screenHeight),
    start(other->start),
    end(other->end),
    compositionNumber(other->compositionNumber),
    numberCompObjects(other->numberCompObjects),
    imageWidth(other->imageWidth),
    imageHeight(other->imageHeight),
    xOfs(other->xOfs),
    yOfs(other->yOfs),
    forced(other->forced),
    decoded(other->decoded),
    excluded(other->excluded)
{
}

SubPicture* SubPicture::copy()
{
    SubPicture* sp = new SubPicture;
    sp->screenHeight = screenHeight;
    sp->screenWidth = screenWidth;
    sp->start = start;
    sp->end = end;
    sp->forced = forced;
    sp->compositionNumber = compositionNumber;
    sp->numberCompObjects = numberCompObjects;
    sp->setImageWidth(imageWidth);
    sp->setImageHeight(imageHeight);
    sp->setOfsX(xOfs);
    sp->setOfsY(xOfs);
    sp->excluded = excluded;
    sp->decoded = decoded;
    if (!erasePatch.empty())
    {
        for (int i = 0; i < erasePatch.size(); ++i)
        {
            sp->erasePatch.push_back(new ErasePatch(erasePatch[i]));
        }
    }

    return sp;
}
