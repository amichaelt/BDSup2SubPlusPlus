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

#include "supdvd.h"
#include "subtitleprocessor.h"
#include "Tools/filebuffer.h"
#include "imageobjectfragment.h"
#include "subpicturedvd.h"
#include "bitmap.h"

SupDVD::SupDVD(QString supFileName, QString ifoFileName, SubtitleProcessor* subtitleProcessor) :
    supFileName(supFileName),
    ifoFileName(ifoFileName)
{
    this->subtitleProcessor = subtitleProcessor;
}

Palette *SupDVD::getPalette()
{
    return palette;
}

Bitmap *SupDVD::getBitmap()
{
    return bitmap;
}

QImage *SupDVD::getImage()
{
    return bitmap->getImage(palette);
}

QImage *SupDVD::getImage(Bitmap *bitmap)
{
    //TODO: Finish implementing
    throw 10;
}

int SupDVD::getPrimaryColorIndex()
{
    return primaryColorIndex;
}

void SupDVD::decode(int index)
{
    if (index < subPictures.size())
    {
        SubstreamDVD::decode(subPictures.at(index), subtitleProcessor);
    }
    else
    {
        //TODO: print error
    }
}

int SupDVD::getNumFrames()
{
    return subPictures.size();
}

int SupDVD::getNumForcedFrames()
{
    //TODO: Finish implementing
    throw 10;
}

bool SupDVD::isForced(int index)
{
    //TODO: Finish implementing
    throw 10;
}

void SupDVD::close()
{
    fileBuffer->close();
}

long SupDVD::getEndTime(int index)
{
    //TODO: Finish implementing
    throw 10;
}

long SupDVD::getStartTime(int index)
{
    //TODO: Finish implementing
    throw 10;
}

long SupDVD::getStartOffset(int index)
{
    return subPictures.at(index)->offset;
}

SubPicture *SupDVD::getSubPicture(int index)
{
    return subPictures.at(index);
}

QVector<int> SupDVD::getFrameAlpha(int index)
{
    //TODO: Finish implementing
    throw 10;
}

QVector<int> SupDVD::getFramePal(int index)
{
    //TODO: Finish implementing
    throw 10;
}

QVector<int> SupDVD::getOriginalFrameAlpha(int index)
{
    //TODO: Finish implementing
    throw 10;
}

QVector<int> SupDVD::getOriginalFramePal(int index)
{
    //TODO: Finish implementing
    throw 10;
}

Palette *SupDVD::getSrcPalette()
{
    return srcPalette;
}

void SupDVD::setSrcPalette(Palette *palette)
{
    //TODO: Finish implementing
    throw 10;
}

int SupDVD::getLanguageIdx()
{
    return languageIdx;
}

void SupDVD::readIfo()
{
    fileBuffer = new FileBuffer(ifoFileName);
    QVector<uchar> header(IFOheader.size());

    fileBuffer->getBytes(0, header, IFOheader.size());

    for (int i = 0; i < IFOheader.size(); ++i)
    {
        if (header[i] != IFOheader[i])
        {
            //TODO: error handling
            throw 10;
        }
    }

    // video attributes
    int videoAttributes = fileBuffer->getWord(0x200);
    if ((videoAttributes & 0x3000) != 0)
    {
        // PAL
        switch ((videoAttributes>>3) & 3)
        {
        case 0:
        {
            screenWidth = 720;
            screenHeight = 576;
        } break;
        case 1:
        {
            screenWidth = 704;
            screenHeight = 576;
        } break;
        case 2:
        {
            screenWidth = 352;
            screenHeight = 576;
        } break;
        case 3:
        {
            screenWidth = 352;
            screenHeight = 288;
        } break;
        }
    }
    else
    {
        // NTSC
        switch ((videoAttributes>>3) & 3)
        {
        case 0:
        {
            screenWidth = 720;
            screenHeight = 480;
        } break;
        case 1:
        {
            screenWidth = 704;
            screenHeight = 480;
        } break;
        case 2:
        {
            screenWidth = 352;
            screenHeight = 480;
        } break;
        case 3:
        {
            screenWidth = 352;
            screenHeight = 240;
        } break;
        }
    }
    //TODO: print message

    // get start offset of Titles&Chapters table
    long VTS_PGCITI_ofs = fileBuffer->getDWord(0xCC) * 2048;

    // get language index of subtitle streams (ignore all but first language)
    if (fileBuffer->getWord(0x254) > 0 && fileBuffer->getByte(0x256) == 1)
    {
        QString language;
        bool found = false;
        language += ((char)fileBuffer->getByte(0x258));
        language += ((char)fileBuffer->getByte(0x259));
        auto languages = subtitleProcessor->getLanguages();
        for (int i=0; i< languages.size(); ++i)
        {
            if (language.toLower() == languages[i][1])
            {
                languageIdx = i;
                found = true;
                break;
            }
        }
        if (!found)
        {
            //TODO: print warning
        }
        else
        {
            //TODO: print message
        }
    }
    else
    {
        //TODO: print message
    }

    // PTT_SRPTI
    VTS_PGCITI_ofs += fileBuffer->getDWord(VTS_PGCITI_ofs+0x0C);
    //TODO: print message

    // assume palette in VTS_PGC_1
    long index = VTS_PGCITI_ofs;
    for (int i = 0; i < 16; ++i)
    {
        int y  = fileBuffer->getByte(index + 0xA4 + 4 * i + 1) & 0xff;
        int cb = fileBuffer->getByte(index + 0xA4 + 4 * i + 2) & 0xff;
        int cr = fileBuffer->getByte(index + 0xA4 + 4 * i + 3) & 0xff;
        srcPalette->setYCbCr(i, y, cb, cr);
    }
}

void SupDVD::readAllSupFrames()
{
    long ofs = 0;
    fileBuffer = new FileBuffer(supFileName);
    long size = fileBuffer->getSize();

    emit maxProgressChanged(size);

    int i = 0;
    do
    {
        //TODO: print current subtitle
        ++i;
        emit currentProgressChanged(i);

        //TODO: print offset

        ofs = readSupFrame(ofs);
    } while (ofs < size);
}

long SupDVD::readSupFrame(long ofs)
{
    long ctrlOfs = -1;
    int  ctrlOfsRel = 0;
    int  rleSize = 0;
    int  ctrlSize = -1;
    ImageObjectFragment* rleFrag;
    int  length;

    // 2 uchars:  packet identifier 0x5350
    long startOfs = ofs;
    if (fileBuffer->getWord(ofs) != 0x5350)
    {
        //TODO: add error handling
        throw 10;
    }
    // 8 uchars PTS:  system clock reference, but use only the first 4
    SubPictureDVD* pic = new SubPictureDVD();
    pic->offset = ofs;
    pic->width = screenWidth;
    pic->height = screenHeight;

    int pts = fileBuffer->getDWordLE(ofs += 2);
    pic->startTime = pts + delayGlob;
    // 2 uchars:  packet length (number of uchars after this entry)
    length = fileBuffer->getWord(ofs += 8);
    // 2 uchars: offset to control buffer
    ctrlOfsRel = fileBuffer->getWord(ofs += 2);
    rleSize = ctrlOfsRel - 2;				// calculate size of RLE buffer
    ctrlSize = (length - ctrlOfsRel) - 2;		// calculate size of control header
    if (ctrlSize < 0)
    {
        //TODO: add error handling
        throw 10;
    }
    ctrlOfs = ctrlOfsRel + ofs;			// absolute offset of control header
    ofs += 2;
    pic->rleFragments = QVector<ImageObjectFragment*>();
    rleFrag = new ImageObjectFragment();
    rleFrag->imageBufferOfs = ofs;
    rleFrag->imagePacketSize = rleSize;
    pic->rleFragments.push_back(rleFrag);
    pic->rleSize = rleSize;

    pic->pal = QVector<int>(4);
    pic->alpha = QVector<int>(4);
    int alphaSum = 0;
    QVector<int> alphaUpdate(4);
    int alphaUpdateSum;
    int delay = -1;
    bool ColAlphaUpdate = false;

    //TODO: print message

    // copy control header in buffer (to be more compatible with VobSub)
    QVector<uchar> ctrlHeader(ctrlSize);
    for (int i = 0; i < ctrlSize; ++i)
    {
        ctrlHeader.replace(i, (uchar)fileBuffer->getByte(ctrlOfs + i));
    }

    int b;
    int index = 0;

    int endSeqOfs = (((ctrlHeader[index + 1] & 0xff) | ((ctrlHeader[index] & 0xff) << 8)) - ctrlOfsRel) - 2;
    if (endSeqOfs < 0 || endSeqOfs > ctrlSize)
    {
        //TODO: print warning
        endSeqOfs = ctrlSize;
    }
    index += 2;
    while (index < endSeqOfs)
    {
        int cmd = ctrlHeader[index++] & 0xff;
        switch (cmd)
        {
        case 0: // forced (?)
        {
            pic->isForced = true;
            numForcedFrames++;
        } break;
        case 1: // start display
            break;
        case 3: // palette info
        {
            b = ctrlHeader[index++] & 0xff;
            pic->pal.replace(3, (b >> 4));
            pic->pal.replace(2, b & 0x0f);
            b = ctrlHeader[index++] & 0xff;
            pic->pal.replace(1, (b >> 4));
            pic->pal.replace(0, b & 0x0f);
            //TODO: print message
        } break;
        case 4: // alpha info
        {
            b = ctrlHeader[index++] & 0xff;
            pic->alpha.replace(3, (b >> 4));
            pic->alpha.replace(2, b & 0x0f);
            b = ctrlHeader[index++] & 0xff;
            pic->alpha.replace(1, (b >> 4));
            pic->alpha.replace(0, b & 0x0f);
            for (int i = 0; i < 4; i++)
            {
                alphaSum += pic->alpha[i] & 0xff;
            }
            //TODO: print message
        } break;
        case 5: // coordinates
        {
            int xOfs = ((ctrlHeader[index] & 0xff) << 4) | ((ctrlHeader[index + 1] & 0xff) >> 4);
            pic->setOfsX(ofsXglob + xOfs);
            pic->setImageWidth((((((ctrlHeader[index + 1] & 0xff) & 0xf) << 8) | (ctrlHeader[index+2] & 0xff)) - xOfs) + 1);

            int yOfs = ((ctrlHeader[index + 3] & 0xff) << 4) | ((ctrlHeader[index + 4] & 0xff) >> 4);
            pic->setOfsY(ofsYglob + yOfs);
            pic->setImageHeight((((((ctrlHeader[index + 4] & 0xff) & 0xf) << 8) | ((ctrlHeader[index + 5] & 0xff))) - yOfs) + 1);

            //TODO: print message
            index += 6;
        } break;
        case 6: // offset to RLE buffer
        {
            pic->evenOfs = ((ctrlHeader[index + 1] & 0xff) | ((ctrlHeader[index] & 0xff) << 8)) - 4;
            pic->oddOfs  = ((ctrlHeader[index + 3] & 0xff) | ((ctrlHeader[index + 2] & 0xff) << 8)) - 4;
            index += 4;
            //TODO: print message
        } break;
        case 7: // color/alpha update
        {
            ColAlphaUpdate = true;
            //int len = ToolBox.getWord(ctrlHeader, index);
            // ignore the details for now, but just get alpha and palette info
            alphaUpdateSum = 0;
            b = ctrlHeader[index + 10] & 0xff;
            alphaUpdate.replace(3, (b >> 4));
            alphaUpdate.replace(2, b & 0x0f);
            b = ctrlHeader[index + 11] & 0xff;
            alphaUpdate.replace(1, (b >> 4));
            alphaUpdate.replace(0, b & 0x0f);
            for (int i = 0; i < 4; i++)
            {
                alphaUpdateSum += alphaUpdate[i] & 0xff;
            }
            // only use more opaque colors
            if (alphaUpdateSum > alphaSum)
            {
                alphaSum = alphaUpdateSum;
                for (int i = 0; i < 4; i++)
                {
                    pic->alpha.replace(i, alphaUpdate[i]);
                }
                // take over frame palette
                b = ctrlHeader[index + 8] & 0xff;
                pic->pal.replace(3, (b >> 4));
                pic->pal.replace(2, b & 0x0f);
                b = ctrlHeader[index + 9] & 0xff;
                pic->pal.replace(1, (b >> 4));
                pic->pal.replace(0, b & 0x0f);
            }
            // search end sequence
            index = endSeqOfs;
            delay = ((ctrlHeader[index + 1] & 0xff) | ((ctrlHeader[index] & 0xff) << 8)) * 1024;
            endSeqOfs = (((ctrlHeader[index + 3] & 0xff) | ((ctrlHeader[index + 2] & 0xff) << 8)) - ctrlOfsRel) - 2;
            if (endSeqOfs < 0 || endSeqOfs > ctrlSize)
            {
                //TODO: print warning
                endSeqOfs = ctrlSize;
            }
            index += 4;
        } break;
        case 0xff: // end sequence
            goto parse_ctrl;
        default:
        {
            //TODO: print warning
        } break;
        }
    }

    parse_ctrl:

    if (endSeqOfs != ctrlSize)
    {
        int ctrlSeqCount = 1;
        index = -1;
        int nextIndex = endSeqOfs;
        while (nextIndex != index)
        {
            index = nextIndex;
            delay = ((ctrlHeader[index + 1] & 0xff) | ((ctrlHeader[index] & 0xff) << 8)) * 1024;
            nextIndex = (((ctrlHeader[index + 3] & 0xff) | ((ctrlHeader[index + 2] & 0xff) << 8)) - ctrlOfsRel) - 2;
            ctrlSeqCount++;
        }
        if (ctrlSeqCount > 2)
        {
            //TODO: print warning
        }
        pic->endTime = pic->startTime + delay;
    }
    else
    {
        pic->endTime = pic->startTime;
    }

    pic->setOriginal();

    if (ColAlphaUpdate)
    {
        //TODO: print warning
    }

    if (alphaSum == 0)
    {
        if (subtitleProcessor->getFixZeroAlpha())
        {
            for (int i=0; i < 4; ++i)
            {
                pic->alpha[i] = lastAlpha[i];
            }
            //TODO: print warning
        }
        else
        {
            //TODO: print warning
        }
    }

    lastAlpha = pic->alpha;

    subPictures.push_back(pic);
    return startOfs + length + 0x0a;
}
