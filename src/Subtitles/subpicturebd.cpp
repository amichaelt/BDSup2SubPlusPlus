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

#include "subpicturebd.h"

SubPictureBD::SubPictureBD()
{
}

SubPictureBD::SubPictureBD(const SubPictureBD *other) :
    SubPicture(other),
    type(other->type),
    WindowSizes(other->WindowSizes),
    numWindows(other->numWindows)
{
    for (int i = 0; i < other->imageObjectList.size(); ++i)
    {
        imageObjectList.push_back(ImageObject(other->imageObjectList[i]));
    }
    palettes.resize(other->palettes.size());
    for (int i = 0; i < other->palettes.size(); ++i)
    {
        for (int j = 0; j < other->palettes[i].size(); ++j)
        {
            palettes[i].push_back(PaletteInfo(other->palettes[i][j]));
        }
    }
}

SubPictureBD::SubPictureBD(const SubPictureBD &other) :
    SubPicture(other),
    type(other.type),
    WindowSizes(other.WindowSizes),
    numWindows(other.numWindows)
{
    for (int i = 0; i < other.imageObjectList.size(); ++i)
    {
        imageObjectList.push_back(ImageObject(other.imageObjectList[i]));
    }
    palettes.resize(other.palettes.size());
    for (int i = 0; i < other.palettes.size(); ++i)
    {
        for (int j = 0; j < other.palettes[i].size(); ++j)
        {
            palettes[i].push_back(PaletteInfo(other.palettes[i][j]));
        }
    }
}

SubPicture* SubPictureBD::copy()
{
    return new SubPictureBD(this);
}
