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

#include "subpicturedvd.h"

SubPictureDVD::SubPictureDVD()
{

}

SubPictureDVD::SubPictureDVD(const SubPictureDVD &other) :
    SubPicture(other),
    pictureOffset(other.pictureOffset),
    size(other.size),
    evenOfs(other.evenOfs),
    oddOfs(other.oddOfs),
    origWidth(other.origWidth),
    origHeight(other.origHeight),
    origX(other.origX),
    origY(other.origY),
    rleFragments(other.rleFragments),
    originalAlpha(other.originalAlpha),
    originalPal(other.originalPal),
    alpha(other.alpha),
    pal(other.pal)
{

}

SubPictureDVD::SubPictureDVD(const SubPictureDVD *other) :
    SubPicture(other),
    pictureOffset(other->pictureOffset),
    size(other->size),
    evenOfs(other->evenOfs),
    oddOfs(other->oddOfs),
    origWidth(other->origWidth),
    origHeight(other->origHeight),
    origX(other->origX),
    origY(other->origY),
    rleFragments(other->rleFragments),
    originalAlpha(other->originalAlpha),
    originalPal(other->originalPal),
    alpha(other->alpha),
    pal(other->pal)
{

}

void SubPictureDVD::setOriginal()
{
    origWidth = imageWidth();
    origHeight = imageHeight();
    origX = x();
    origY = y();

    originalAlpha = alpha;
    originalPal = pal;
}

void SubPictureDVD::copyInfo(SubPicture &subPicture)
{
    setScreenWidth(subPicture.screenWidth());
    setScreenHeight(subPicture.screenHeight());
    setStartTime(subPicture.startTime());
    setEndTime(subPicture.endTime());
    setForced(subPicture.isForced());
    setCompNum(subPicture.compNum());
    setImageWidth(subPicture.imageWidth());
    setImageHeight(subPicture.imageHeight());
    setX(subPicture.x());
    setY(subPicture.y());
}
