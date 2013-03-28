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
    paletteUpdate(other->paletteUpdate),
    compState(other->compState),
    paletteID(other->paletteID)
{
    for (ImageObject imageObject : other->imageObjectList)
    {
        imageObjectList[imageObject.objectID()] = imageObject;
    }
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
    paletteUpdate(other.paletteUpdate),
    compState(other.compState),
    paletteID(other.paletteID)
{
    for (ImageObject imageObject : other.imageObjectList)
    {
        imageObjectList[imageObject.objectID()] = imageObject;
    }
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

void SubPictureBD::setData(PCS pcs, QMap<int, QVector<ODS>> ods, QMap<int, QVector<PaletteInfo>> pds, WDS wds)
{
    start = pcs.pts;
    _screenWidth = pcs.videoWidth;
    _screenHeight = pcs.videoHeight;
    type = pcs.frameRate;
    compositionNumber = pcs.compositionNumber;
    compState = pcs.compositionState;
    paletteUpdate = pcs.paletteUpdate;
    paletteID = pcs.paletteId;
    numberCompObjects = pcs.numberOfCompositionObjects;

    QMap<int, QRect> imageRects;

    objectIds = pcs.objectIds;

    for (int objectId : objectIds)
    {
        ImageObject object;
        if (ods[objectId][0].width == 0 || ods[objectId][0].height == 0)
        {
            continue;
        }
        object.setObjectID(objectId);
        object.setWindowID(pcs.windowIds[objectId]);
        object.setForcedFlags(pcs.forcedFlags[objectId]);
        object.setX(pcs.xPositions[objectId]);
        object.setY(pcs.yPositions[objectId]);
        object.setWidth(ods[objectId][0].width);
        object.setHeight(ods[objectId][0].height);
        object.setObjectVersion(ods[objectId][0].objectVersion);
        for (ODS odsData : ods[objectId])
        {
            object.setBufferSize(object.bufferSize() + odsData.fragment.imagePacketSize());
            object.fragmentList().push_back(odsData.fragment);
        }
        imageObjectList[objectId] = object;
        imageRects[objectId] = QRect(pcs.xPositions[objectId], pcs.yPositions[objectId],
                                     ods[objectId][0].width, ods[objectId][0].height);
    }
    palettes = pds;

    numWindows = wds.numberOfWindows;
    setWindowSizes(wds.windows);
    setImageSizes(imageRects);
}
