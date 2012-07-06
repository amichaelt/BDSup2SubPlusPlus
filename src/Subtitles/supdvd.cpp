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

#include "supdvd.h"
#include "subtitleprocessor.h"
#include "Tools/filebuffer.h"
#include "Tools/numberutil.h"
#include "imageobjectfragment.h"
#include "subpicturedvd.h"
#include "bitmap.h"
#include "palette.h"

SupDVD::SupDVD(QString supFileName, QString ifoFileName, SubtitleProcessor* subtitleProcessor) :
    supFileName(supFileName),
    ifoFileName(ifoFileName)
{
    this->subtitleProcessor = subtitleProcessor;
}

QImage *SupDVD::getImage()
{
    return bitmap->getImage(palette);
}

QImage *SupDVD::getImage(Bitmap *bitmap)
{
    return bitmap->getImage(palette);
}

void SupDVD::decode(int index)
{
    if (index < subPictures.size())
    {
        SubstreamDVD::decode(subPictures.at(index), subtitleProcessor);
    }
    else
    {
        throw QString("Index: %1 out of bounds.\n").arg(QString::number(index));
    }
}

int SupDVD::getNumFrames()
{
    return subPictures.size();
}

bool SupDVD::isForced(int index)
{
    return subPictures.at(index)->isForced;
}

long SupDVD::getEndTime(int index)
{
    return subPictures.at(index)->endTime;
}

long SupDVD::getStartTime(int index)
{
    return subPictures.at(index)->startTime;
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
    return subPictures.at(index)->alpha;
}

QVector<int> SupDVD::getFramePal(int index)
{
    return subPictures.at(index)->pal;
}

QVector<int> SupDVD::getOriginalFrameAlpha(int index)
{
    return subPictures.at(index)->originalAlpha;
}

QVector<int> SupDVD::getOriginalFramePal(int index)
{
    return subPictures.at(index)->originalPal;
}

void SupDVD::readIfo()
{
    fileBuffer.reset(new FileBuffer(ifoFileName));

    QVector<uchar> header(IFOheader.size());

    fileBuffer->getBytes(0, header, IFOheader.size());

    for (int i = 0; i < IFOheader.size(); ++i)
    {
        if (header[i] != IFOheader[i])
        {
            throw QString("Not a valid IFO file.");
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

    subtitleProcessor->print(QString("Resolution: %1x%2\n").arg(QString::number(screenWidth))
                                                           .arg(QString::number(screenHeight)));

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
        for (int i = 0; i< languages.size(); ++i)
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
            subtitleProcessor->printWarning(QString("Illegal language id: %1\n").arg(language));
        }
        else
        {
            subtitleProcessor->print(QString("Set language to: %1\n").arg(language));
        }
    }
    else
    {
        subtitleProcessor->printWarning(QString("Missing language id.\n"));
    }

    // PTT_SRPTI
    VTS_PGCITI_ofs += fileBuffer->getDWord(VTS_PGCITI_ofs + 0x0C);

    subtitleProcessor->print(QString("Reading palette from offset: %1\n").arg(QString::number(VTS_PGCITI_ofs, 16), 8, QChar('0')));

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

void SupDVD::writeIfo(QString filename, SubPicture *subPicture, Palette *palette)
{
    QVector<uchar> buf(0x1800);
    int index = 0;

    // video attributes
    int vidAttr;
    if (subPicture->height == 480)
    {
        vidAttr = 0x4f01; // NTSC
    }
    else
    {
        vidAttr = 0x5f01; // PAL
    }

    // VTSI_MAT

    NumberUtil::setString(buf, index, "DVDVIDEO-VTS");
    NumberUtil::setDWord(buf, index + 0x12, 0x00000004);            // last sector of title set
    NumberUtil::setDWord(buf, index + 0x1C, 0x00000004);            // last sector of IFO
    NumberUtil::setDWord(buf, index + 0x80, 0x000007ff);            // end byte address of VTS_MAT
    NumberUtil::setDWord(buf, index + 0xC8, 0x00000001);            // start sector of Title Vob (*2048 -> 0x0800) -> PTT_SRPTI
    NumberUtil::setDWord(buf, index + 0xCC, 0x00000002);            // start sector of Titles&Chapters table (*2048 -> 0x1000) -> VTS_PGCITI

    NumberUtil::setWord(buf, index + 0x100, vidAttr);               // video attributes
    NumberUtil::setWord(buf, index + 0x200, vidAttr);               // video attributes

    QString l = subtitleProcessor->getLanguages()[subtitleProcessor->getLanguageIdx()][1];
    NumberUtil::setWord(buf, index + 0x254, 1);                     // number of subtitle streams
    NumberUtil::setByte(buf, index + 0x256, 1);                     // subtitle attributes
    NumberUtil::setByte(buf, index + 0x258, l.at(0).toAscii());
    NumberUtil::setByte(buf, index + 0x259, l.at(1).toAscii());

    // PTT_SRPTI
    index = 0x0800;
    NumberUtil::setWord(buf, index, 0x0001);                        // Number of TTUs
    NumberUtil::setWord(buf, index + 0x04, 0x000f);                 // End byte of PTT_SRPT table
    NumberUtil::setDWord(buf, index + 0x04, 0x0000000C);            // TTU_1: starting byte
    NumberUtil::setWord(buf, index + 0x0C, 0x0001);                 // PTT_1: program chain number PGCN
    NumberUtil::setWord(buf, index + 0x0e, 0x0001);                 // PTT_1: program number PG

    // VTS_PGCITI/VTS_PTT_SRPT
    index = 0x1000;
    NumberUtil::setWord(buf, index, 0x0001);                        // Number of VTS_PGCI_SRP (2 bytes, 2 bytes reserved)
    NumberUtil::setDWord(buf, index + 0x04, 0x00000119);            // end byte of VTS_PGCI_SRP table (281)
    NumberUtil::setDWord(buf, index + 0x08, 0x81000000);            // VTS_PGC_1_ category mask. entry PGC (0x80), title number 1 (0x01), Category 0,...
    NumberUtil::setDWord(buf, index + 0x0C, 0x00000010);            // VTS_PGCI start byte (16)

    // VTS_PGC_1
    index = 0x1010;
    NumberUtil::setByte(buf, index + 0x02, 0x01);                   // Number of Programs
    NumberUtil::setByte(buf, index + 0x03, 0x01);                   // Number of Cells
    for (int i = 0; i < 16; ++i)
    {
        QVector<int> ycbcr = palette->getYCbCr(i);
        NumberUtil::setByte(buf, index + 0xA4 + (4 * i) + 1, ycbcr[0]);
        NumberUtil::setByte(buf, index + 0xA4 + (4 * i) + 2, ycbcr[1]);
        NumberUtil::setByte(buf, index + 0xA4 + (4 * i) + 3, ycbcr[2]);
    }
    QScopedPointer<QFile> out(new QFile(filename));
    if (!out->open(QIODevice::WriteOnly))
    {
        throw QString("File: '%1' can not be opened for writing.").arg(filename);
    }
    out->write((char*)buf.data(), buf.size());
}

void SupDVD::readAllSupFrames()
{
    fileBuffer.reset(new FileBuffer(supFileName));

    long ofs = 0;
    long size = fileBuffer->getSize();

    if (size <= 0)
    {
        throw QString("SUP file is empty. No subtitles can be read.");
    }

    emit maxProgressChanged(size);

    int i = 0;
    do
    {
        subtitleProcessor->printX(QString("# %1\n").arg(++i));

        emit currentProgressChanged(i);

        subtitleProcessor->print(QString("Ofs: %1\n").arg(QString::number(ofs, 16), 8, QChar('0')));

        ofs = readSupFrame(ofs);
    } while (ofs < size);
    emit currentProgressChanged(i);

    subtitleProcessor->printX(QString("\nDetected %1 forced captions.\n").arg(QString::number(numForcedFrames)));
}

QVector<uchar> SupDVD::createSupFrame(SubPictureDVD *subPicture, Bitmap *bitmap)
{
    /* create RLE buffers */
    QVector<uchar> even = encodeLines(bitmap, true);
    QVector<uchar> odd  = encodeLines(bitmap, false);
    int tmp;

    int forcedOfs;
    int controlHeaderLen;
    if (subPicture->isForced)
    {
        forcedOfs = 0;
        controlHeader.replace(2, 0x01); // display
        controlHeader.replace(3, 0x00); // forced
        controlHeaderLen = controlHeader.size();

    }
    else
    {
        forcedOfs = 1;
        controlHeader.replace(2, 0x00); // part of offset
        controlHeader.replace(3, 0x01); // display
        controlHeaderLen = controlHeader.size() - 1;
    }

    // fill out all info but the offets (determined later)
    int sizeRLE = even.size() + odd.size();
    int bufSize = 10 + 4 + controlHeaderLen + sizeRLE;
    QVector<uchar> buf(bufSize);

    // write header
    buf.replace(0, 0x53);
    buf.replace(1, 0x50);
    // write PTS (4 bytes of 8 bytes used) - little endian!
    int pts = (int)subPicture->startTime;
    buf.replace(5, (uchar)(pts >> 24));
    buf.replace(4, (uchar)(pts >> 16));
    buf.replace(3, (uchar)(pts >> 8));
    buf.replace(2, (uchar)pts);

    // write packet size
    tmp = controlHeaderLen + sizeRLE + 4; // 4 for the size and the offset
    buf.replace(10, (uchar)(tmp >> 8));
    buf.replace(11, (uchar)(tmp));

    // write offset to control header +
    tmp = sizeRLE + 2; // 2 for the offset
    buf.replace(12, (uchar)(tmp >> 8));
    buf.replace(13, (uchar)(tmp));

    // copy rle buffers
    int ofs = 14;
    for (int i = 0; i < even.size(); ++i)
    {
        buf.replace(ofs++, even[i]);
    }
    for (int i = 0; i < odd.size(); ++i)
    {
        buf.replace(ofs++, odd[i]);
    }

    /* create control header */
    /* palette (store reversed) */
    controlHeader.replace(1 + 4, (uchar)(((subPicture->pal[3] & 0xf) << 4) | (subPicture->pal[2] & 0x0f)));
    controlHeader.replace(1 + 5, (uchar)(((subPicture->pal[1] & 0xf) << 4) | (subPicture->pal[0] & 0x0f)));
    /* alpha (store reversed) */
    controlHeader.replace(1 + 7, (uchar)(((subPicture->alpha[3] & 0xf) << 4) | (subPicture->alpha[2] & 0x0f)));
    controlHeader.replace(1 + 8, (uchar)(((subPicture->alpha[1] & 0xf) << 4) | (subPicture->alpha[0] & 0x0f)));

    /* coordinates of subtitle */
    controlHeader.replace(1 + 10, (uchar)((subPicture->getOfsX() >> 4) & 0xff));
    tmp = subPicture->getOfsX()+bitmap->getWidth()-1;
    controlHeader.replace(1 + 11, (uchar)(((subPicture->getOfsX() & 0xf) << 4) | ((tmp >> 8) & 0xf)));
    controlHeader.replace(1 + 12, (uchar)(tmp & 0xff));

    int yOfs = subPicture->getOfsY() - subtitleProcessor->getCropOfsY();
    if (yOfs < 0)
    {
        yOfs = 0;
    }
    else
    {
        int yMax = (subPicture->height - subPicture->getImageHeight()) - (2 * subtitleProcessor->getCropOfsY());
        if (yOfs > yMax)
        {
            yOfs = yMax;
        }
    }

    controlHeader.replace(1 + 13, (uchar)((yOfs >> 4) & 0xff));
    tmp = (yOfs + bitmap->getHeight()) - 1;
    controlHeader.replace(1 + 14, (uchar)(((yOfs & 0xf) << 4) | ((tmp >> 8) & 0xf)));
    controlHeader.replace(1 + 15, (uchar)(tmp & 0xff));

    /* offset to even lines in rle buffer */
    controlHeader.replace(1 + 17, 0x00); /* 2 bytes subpicture size and 2 bytes control header ofs */
    controlHeader.replace(1 + 18, 0x04); /* note: SubtitleCreator uses 6 and adds 0x0000 in between */

    /* offset to odd lines in rle buffer */
    tmp = even.size() + controlHeader[1 + 18];
    controlHeader.replace(1 + 19, (uchar)((tmp >> 8) & 0xff));
    controlHeader.replace(1 + 20, (uchar)(tmp & 0xff));

    /* display duration in frames */
    tmp = (int)((subPicture->endTime-subPicture->startTime) / 1024); // 11.378ms resolution????
    controlHeader.replace(1 + 22, (uchar)((tmp >> 8) & 0xff));
    controlHeader.replace(1 + 23, (uchar)(tmp & 0xff));

    /* offset to end sequence - 22 is the offset of the end sequence */
    tmp = sizeRLE + 22 + (subPicture->isForced ? 1 : 0) + 4;
    controlHeader.replace(forcedOfs + 0, (uchar)((tmp >> 8) & 0xff));
    controlHeader.replace(forcedOfs + 1, (uchar)(tmp & 0xff));
    controlHeader.replace(1 + 24, (uchar)((tmp >> 8) & 0xff));
    controlHeader.replace(1 + 25, (uchar)(tmp & 0xff));

    // write control header
    for (int i = 0; i < controlHeaderLen; ++i)
    {
        buf.replace(ofs++, controlHeader[forcedOfs + i]);
    }

    return buf;
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
        throw QString("Missing packet identifier at ofs: %1").arg(QString::number(ofs, 16), 8, QChar('0'));
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
        throw QString("Invalid control buffer size");
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

    subtitleProcessor->print(QString("SP_DCSQT at ofs: %1\n").arg(QString::number(ctrlOfs, 16), 8, QChar('0')));

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
        subtitleProcessor->printWarning("Invalid end sequence offset -> no end time\n");

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

            subtitleProcessor->print(QString("Palette:   %1, %2, %3, %4\n")
                                     .arg(QString::number(pic->pal[0]))
                                     .arg(QString::number(pic->pal[1]))
                                     .arg(QString::number(pic->pal[2]))
                                     .arg(QString::number(pic->pal[3])));
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

            subtitleProcessor->print(QString("Alpha:     %1, %2, %3, %4\n")
                                     .arg(QString::number(pic->alpha[0]))
                                     .arg(QString::number(pic->alpha[1]))
                                     .arg(QString::number(pic->alpha[2]))
                                     .arg(QString::number(pic->alpha[3])));
        } break;
        case 5: // coordinates
        {
            int xOfs = ((ctrlHeader[index] & 0xff) << 4) | ((ctrlHeader[index + 1] & 0xff) >> 4);
            pic->setOfsX(ofsXglob + xOfs);
            int imageWidth = ((((ctrlHeader[index + 1] & 0xff) & 0xf) << 8) | (ctrlHeader[index+2] & 0xff));
            pic->setImageWidth((imageWidth - xOfs) + 1);

            int yOfs = ((ctrlHeader[index + 3] & 0xff) << 4) | ((ctrlHeader[index + 4] & 0xff) >> 4);
            pic->setOfsY(ofsYglob + yOfs);
            int imageHeight = ((((ctrlHeader[index + 4] & 0xff) & 0xf) << 8) | ((ctrlHeader[index + 5] & 0xff)));
            pic->setImageHeight((imageHeight - yOfs) + 1);

            subtitleProcessor->print(QString("Area info: (%1, %2) - (%3, %4)\n")
                                     .arg(QString::number(pic->getOfsX()))
                                     .arg(QString::number(pic->getOfsY()))
                                     .arg(QString::number((pic->getOfsX() + pic->getImageWidth()) - 1))
                                     .arg(QString::number((pic->getOfsY() + pic->getImageHeight()) - 1)));

            index += 6;
        } break;
        case 6: // offset to RLE buffer
        {
            pic->evenOfs = ((ctrlHeader[index + 1] & 0xff) | ((ctrlHeader[index] & 0xff) << 8)) - 4;
            pic->oddOfs  = ((ctrlHeader[index + 3] & 0xff) | ((ctrlHeader[index + 2] & 0xff) << 8)) - 4;
            index += 4;

            subtitleProcessor->print(QString("RLE ofs:   %1, %2\n")
                                     .arg(QString::number(pic->evenOfs, 16), 4, QChar('0'))
                                     .arg(QString::number(pic->oddOfs, 16), 4, QChar('0')));
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
                subtitleProcessor->printWarning(QString("Invalid end sequence offset -> no end time\n"));

                endSeqOfs = ctrlSize;
            }
            index += 4;
        } break;
        case 0xff: // end sequence
            goto parse_ctrl;
        default:
        {
            subtitleProcessor->printWarning(QString("Unknown control sequence %1 skipped\n")
                                            .arg(QString::number(cmd, 16), 2, QChar('0')));
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
            subtitleProcessor->printWarning(QString("Control sequence(s) ignored - result may be erratic.\n"));
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
        subtitleProcessor->printWarning(QString("Palette update/alpha fading detected - result may be erratic.\n"));
    }

    if (alphaSum == 0)
    {
        if (subtitleProcessor->getFixZeroAlpha())
        {
            for (int i=0; i < 4; ++i)
            {
                pic->alpha[i] = lastAlpha[i];
            }

            subtitleProcessor->printWarning(QString("Invisible caption due to zero alpha - used alpha info of last caption.\n"));
        }
        else
        {
            subtitleProcessor->printWarning(QString("Invisible caption due to zero alpha (not fixed due to user setting).\n"));
        }
    }

    lastAlpha = pic->alpha;

    subPictures.push_back(pic);
    return startOfs + length + 0x0a;
}
