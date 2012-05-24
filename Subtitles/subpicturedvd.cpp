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

#include "subpicturedvd.h"

SubPictureDVD::SubPictureDVD() :
    offset(0),
    rleSize(0),
    evenOfs(0),
    oddOfs(0),
    originalWidth(0),
    originalHeight(0),
    originalX(0),
    originalY(0),
    originalAlpha(4),
    originalPal(4),
    alpha(4),
    pal(4)
{
}

void SubPictureDVD::setOriginal()
{
    originalWidth = getImageWidth();
    originalHeight = getImageHeight();
    originalX = getOfsX();
    originalY = getOfsY();

    originalAlpha = alpha;
    originalPal = pal;
}
