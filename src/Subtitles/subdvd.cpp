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

#include "subdvd.h"
#include "bitmap.h"
#include "palette.h"
#include "types.h"
#include "subtitleprocessor.h"
#include "subpicturedvd.h"
#include "Tools/filebuffer.h"
#include "Tools/timeutil.h"

#include <QImage>
#include <QFile>
#include <QRect>
#include <QTextStream>
#include <QFileInfo>
#include <QVector>
#include <QMessageBox>

SubDVD::SubDVD(QString subFileName, QString idxFileName, SubtitleProcessor* subtitleProcessor)
{
    this->subtitleProcessor = subtitleProcessor;
    this->subFileName = subFileName;
    this->idxFileName = idxFileName;
    palette = new Palette(4, true);
    bitmap = new Bitmap(0, 0);
}

QImage *SubDVD::getImage()
{
    return bitmap->getImage(palette);
}

QImage *SubDVD::getImage(Bitmap *bitmap)
{
    return bitmap->getImage(palette);
}

void SubDVD::decode(int index)
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

int SubDVD::getNumFrames()
{
    return subPictures.size();
}

bool SubDVD::isForced(int index)
{
    return subPictures.at(index)->isForced;
}

long SubDVD::getEndTime(int index)
{
    return subPictures.at(index)->endTime;
}

long SubDVD::getStartTime(int index)
{
    return subPictures.at(index)->startTime;
}

long SubDVD::getStartOffset(int index)
{
    return subPictures.at(index)->offset;
}

SubPicture *SubDVD::getSubPicture(int index)
{
    return subPictures.at(index);
}

QVector<int> SubDVD::getFrameAlpha(int index)
{
    return subPictures.at(index)->alpha;
}

QVector<int> SubDVD::getFramePal(int index)
{
    return subPictures.at(index)->pal;
}

QVector<int> SubDVD::getOriginalFrameAlpha(int index)
{
    return subPictures.at(index)->originalAlpha;
}

QVector<int> SubDVD::getOriginalFramePal(int index)
{
    return subPictures.at(index)->originalPal;
}

void SubDVD::readSubFrame(SubPictureDVD *pic, long endOfs)
{
    long ofs = pic->offset;
    long ctrlOfs = -1;
    long nextOfs;
    int  ctrlOfsRel = 0;
    int rleSize = 0;
    int rleBufferFound = 0;
    int ctrlSize = -1;
    int ctrlHeaderCopied = 0;
    QVector<uchar> ctrlHeader;
    ImageObjectFragment* rleFrag;
    int length;
    int packHeaderSize;
    bool firstPackFound = false;

    do {
        // 4 bytes:  packet identifier 0x000001ba
        long startOfs = ofs;
        if (fileBuffer->getDWord(ofs) != 0x000001ba)
        {
            throw QString("Missing packet identifier at ofs %1").arg(QString::number(ofs, 16), 8, QChar('0'));
        }

        // 6 bytes:  system clock reference
        // 3 bytes:  multiplexer rate
        // 1 byte:   stuffing info
        int stuffOfs = fileBuffer->getByte(ofs += 13) & 7;

        // 4 bytes:  sub packet ID 0x000001bd
        if (fileBuffer->getDWord(ofs += (1 + stuffOfs)) != 0x000001bd)
        {
            throw QString("Missing packet identifier at ofs %1").arg(QString::number(ofs, 16), 8, QChar('0'));
        }
        // 2 bytes:  packet length (number of bytes after this entry)
        length = fileBuffer->getWord(ofs += 4);
        nextOfs = ofs + 2 + length;

        // 2 bytes:  packet type
        ofs += 2;
        packHeaderSize = (int)(ofs - startOfs);
        bool firstPack = ((fileBuffer->getByte(++ofs) & 0x80) == 0x80);

        // 1 byte    pts length
        int ptsLength = fileBuffer->getByte(ofs += 1);
        ofs += (1 + ptsLength); // skip PTS and stream ID
        int packetStreamID = fileBuffer->getByte(ofs++) - 0x20;
        if (packetStreamID != streamID)
        {
            // packet doesn't belong to stream -> skip
            if ((nextOfs % 0x800) != 0)
            {
                ofs = ((nextOfs / 0x800) + 1) * 0x800;

                subtitleProcessor->printWarning(QString("Offset to next fragment is invalid. Fixed to: %1\n")
                                                .arg(QString::number(ofs, 16), 8, QChar('0')));
            }
            else
            {
                ofs = nextOfs;
            }
            ctrlOfs += 0x800;
            continue;
        }
        int headerSize = (int)(ofs - startOfs); // only valid for additional packets
        if (firstPack && ptsLength >= 5)
        {
            int size = fileBuffer->getWord(ofs);
            ofs += 2;
            ctrlOfsRel = fileBuffer->getWord(ofs);
            rleSize = ctrlOfsRel - 2;             // calculate size of RLE buffer
            ctrlSize = (size - ctrlOfsRel) - 2;       // calculate size of control header
            if (ctrlSize < 0)
            {
                throw QString("Invalid control buffer size");
            }
            ctrlHeader = QVector<uchar>(ctrlSize);
            ctrlOfs = ctrlOfsRel + ofs; // might have to be corrected for multiple packets
            ofs += 2;
            headerSize = (int)(ofs - startOfs);
            firstPackFound = true;
        }
        else
        {
            if (firstPackFound)
            {
                ctrlOfs += headerSize; // fix absolute offset by adding header bytes
            }
            else
            {
                subtitleProcessor->printWarning(QString("Invalid fragment skipped at ofs %1\n")
                                                .arg(QString::number(startOfs, 16), 8, QChar('0')));
            }
        }

        // check if control header is (partly) in this packet
        int diff = (int)((nextOfs - ctrlOfs) - ctrlHeaderCopied);
        if (diff < 0)
        {
            diff = 0;
        }
        int copied = ctrlHeaderCopied;
        for (int i = 0; (i < diff) && (ctrlHeaderCopied < ctrlSize); ++i)
        {
            ctrlHeader.replace(ctrlHeaderCopied, (uchar)fileBuffer->getByte(ctrlOfs + i + copied));
            ++ctrlHeaderCopied;
        }
        rleFrag = new ImageObjectFragment();
        rleFrag->imageBufferOfs = ofs;
        rleFrag->imagePacketSize = (((length - headerSize) - diff) + packHeaderSize);
        pic->rleFragments.push_back(rleFrag);

        rleBufferFound += rleFrag->imagePacketSize;

        if (ctrlHeaderCopied != ctrlSize && ((nextOfs % 0x800) != 0))
        {
            ofs = ((nextOfs / 0x800) + 1) * 0x800;

            subtitleProcessor->printWarning(QString("Offset to next fragment is invalid. Fixed to: %1")
                                            .arg(QString::number(ofs, 16), 8, QChar('0')));

            rleBufferFound += ofs-nextOfs;
        }
        else
        {
            ofs = nextOfs;
        }
    } while (ofs < endOfs && ctrlHeaderCopied < ctrlSize);

    if (ctrlHeaderCopied != ctrlSize)
    {
        subtitleProcessor->printWarning("Control buffer size inconsistent.\n");

        for (int i = ctrlHeaderCopied; i < ctrlSize; ++i)
        {
            ctrlHeader.replace(i, 0xff);
        }
    }

    if (rleBufferFound != rleSize)
    {
        subtitleProcessor->printWarning("RLE buffer size inconsistent.\n");
    }

    pic->rleSize = rleBufferFound;
    int alphaSum = 0;
    QVector<int> alphaUpdate(4);
    int alphaUpdateSum;
    int delay = -1;
    bool ColAlphaUpdate = false;

    subtitleProcessor->print(QString("SP_DCSQT at ofs: %1\n").arg(QString::number(ctrlOfs, 16), 8, QChar('0')));

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
            pic->pal.replace(2, (b & 0x0f));
            b = ctrlHeader[index++] & 0xff;
            pic->pal.replace(1, (b >> 4));
            pic->pal.replace(0, (b & 0x0f));

            subtitleProcessor->print(QString("Palette:   %1, %2, %3, %4\n")
                                     .arg(QString::number(pic->pal[0])).arg(QString::number(pic->pal[1]))
                                     .arg(QString::number(pic->pal[2])).arg(QString::number(pic->pal[3])));

        } break;
        case 4: // alpha info
        {
            b = ctrlHeader[index++] & 0xff;
            pic->alpha.replace(3, (b >> 4));
            pic->alpha.replace(2, (b & 0x0f));
            b = ctrlHeader[index++] & 0xff;
            pic->alpha.replace(1, (b >> 4));
            pic->alpha.replace(0, (b & 0x0f));
            for (int i = 0; i<4; i++)
            {
                alphaSum += pic->alpha[i] & 0xff;
            }

            subtitleProcessor->print(QString("Alpha:     %1, %2, %3, %4\n")
                                     .arg(QString::number(pic->alpha[0])).arg(QString::number(pic->alpha[1]))
                                     .arg(QString::number(pic->alpha[2])).arg(QString::number(pic->alpha[3])));
        } break;
        case 5: // coordinates
        {
            int xOfs = ((ctrlHeader[index] & 0xff) << 4) | ((ctrlHeader[index + 1] & 0xff) >> 4);
            pic->setOfsX(ofsXglob + xOfs);
            int imageWidth = ((((ctrlHeader[index + 1] & 0xff) & 0xf) << 8) | (ctrlHeader[index + 2] & 0xff));
            pic->setImageWidth((imageWidth - xOfs) + 1);
            int yOfs = ((ctrlHeader[index + 3] & 0xff) << 4) | ((ctrlHeader[index + 4] & 0xff) >> 4);
            pic->setOfsY(ofsYglob + yOfs);
            int imageHeight = ((((ctrlHeader[index + 4] & 0xff) & 0xf) << 8) | (ctrlHeader[index+5] & 0xff));
            pic->setImageHeight((imageHeight - yOfs) + 1);

            subtitleProcessor->print(QString("Area info: (%1, %2) - (%3, %4)\n")
                                     .arg(QString::number(pic->getOfsX())).arg(QString::number(pic->getOfsY()))
                                     .arg(QString::number((pic->getOfsX() + pic->getImageWidth()) - 1))
                                     .arg(QString::number((pic->getOfsY() + pic->getImageHeight()) - 1)));

            index += 6;
        } break;
        case 6: // offset to RLE buffer
        {
            pic->evenOfs = ((ctrlHeader[index + 1] & 0xff) | ((ctrlHeader[index] & 0xff) << 8)) - 4;
            pic->oddOfs = ((ctrlHeader[index + 3] & 0xff) | ((ctrlHeader[index + 2] & 0xff) << 8)) - 4;
            index += 4;

            subtitleProcessor->print(QString("RLE ofs:   %1, %2\n")
                                     .arg(QString::number(pic->evenOfs, 16), 4, QChar('0'))
                                     .arg(QString::number(pic->oddOfs, 16), 4, QChar('0')));
        } break;
        case 7: // color/alpha update
        {
            ColAlphaUpdate = true;
            // ignore the details for now, but just get alpha and palette info
            alphaUpdateSum = 0;
            b = ctrlHeader[index + 10] & 0xff;
            alphaUpdate.replace(3, (b >> 4));
            alphaUpdate.replace(2, (b & 0x0f));
            b = ctrlHeader[index + 11] & 0xff;
            alphaUpdate.replace(1, (b >> 4));
            alphaUpdate.replace(0, (b & 0x0f));
            for (int i = 0; i < 4; i++)
            {
                alphaUpdateSum += alphaUpdate[i] & 0xff;
            }
            // only use more opaque colors
            if (alphaUpdateSum > alphaSum) {
                alphaSum = alphaUpdateSum;
                for (int i = 0; i<4; i++)
                    pic->alpha.replace(i, alphaUpdate[i]);
                // take over frame palette
                b =ctrlHeader[index + 8] & 0xff;
                pic->pal.replace(3, (b >> 4));
                pic->pal.replace(2, (b & 0x0f));
                b = ctrlHeader[index + 9];
                pic->pal.replace(1, (b >> 4));
                pic->pal.replace(0, (b & 0x0f));
            }
            // search end sequence
            index = endSeqOfs;
            delay = ((ctrlHeader[index + 1] & 0xff) | ((ctrlHeader[index] & 0xff) << 8)) *1024;
            endSeqOfs = (((ctrlHeader[index + 3] & 0xff) | ((ctrlHeader[index + 2] & 0xff) << 8)) - ctrlOfsRel) - 2;
            if (endSeqOfs < 0 || endSeqOfs > ctrlSize)
            {
                subtitleProcessor->printWarning("Invalid end sequence offset -> no end time\n");

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
            subtitleProcessor->printWarning("Control sequence(s) ignored - result may be erratic.");
        }
        pic->endTime = pic->startTime + delay;
    }
    else
    {
        pic->endTime = pic->startTime;
    }

    if (ColAlphaUpdate)
    {
        subtitleProcessor->printWarning("Palette update/alpha fading detected - result may be erratic.\n");
    }

    if (alphaSum == 0)
    {
        if (subtitleProcessor->getFixZeroAlpha())
        {
            for (int i=0; i < 4; i++)
            {
                pic->alpha.replace(i, lastAlpha[i]);
            }

            subtitleProcessor->printWarning("Invisible caption due to zero alpha - used alpha info of last caption.\n");
        }
        else
        {
            subtitleProcessor->printWarning("Invisible caption due to zero alpha (not fixed due to user setting).\n");
        }
    }

    lastAlpha = pic->alpha;

    pic->setOriginal();
}

void SubDVD::readAllSubFrames()
{
    fileBuffer.reset(new FileBuffer(subFileName));

    for (int i = 0; i < subPictures.size(); ++i)
    {
        emit currentProgressChanged(i);

        subtitleProcessor->printX(QString("# %1\n").arg(QString::number(i + 1)));

        long nextOfs;
        if (i < subPictures.size() - 1)
        {
            nextOfs = subPictures.at(i + 1)->offset;
        }
        else
        {
            nextOfs = fileBuffer->getSize();
        }
        readSubFrame(subPictures.at(i), nextOfs);
    }

    emit currentProgressChanged(subPictures.size());

    subtitleProcessor->printX(QString("\nDetected %1 forced captions.\n").arg(QString::number(numForcedFrames)));
}

QVector<uchar> SubDVD::createSubFrame(SubPictureDVD *subPicture, Bitmap *bitmap)
{
    QVector<uchar> even = encodeLines(bitmap, true);
    QVector<uchar> odd = encodeLines(bitmap, false);
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

    /* header - contains PTM */
    int ptm = (int)subPicture->startTime; // should be end time, but STC writes start time?
    headerFirst.replace(9, (uchar)(((ptm >> 29) & 0x0E) | 0x21));
    headerFirst.replace(10, (uchar)(ptm >> 22));
    headerFirst.replace(11, (uchar)((ptm >> 14) | 1));
    headerFirst.replace(12, (uchar)(ptm >> 7));
    headerFirst.replace(13, (uchar)((ptm * 2) + 1));

    /* control header */
    /* palette (store reversed) */
    controlHeader.replace(1 + 4, (uchar)(((subPicture->pal[3] & 0xf) << 4) | (subPicture->pal[2] & 0x0f)));
    controlHeader.replace(1 + 5, (uchar)(((subPicture->pal[1] & 0xf) << 4) | (subPicture->pal[0] & 0x0f)));
    /* alpha (store reversed) */
    controlHeader.replace(1 + 7, (uchar)(((subPicture->alpha[3] & 0xf) << 4) | (subPicture->alpha[2] & 0x0f)));
    controlHeader.replace(1 + 8, (uchar)(((subPicture->alpha[1] & 0xf) << 4) | (subPicture->alpha[0] & 0x0f)));

    /* coordinates of subtitle */
    controlHeader.replace(1 + 10, (uchar)((subPicture->getOfsX() >> 4) & 0xff));
    tmp = (subPicture->getOfsX() + bitmap->getWidth()) - 1;
    controlHeader.replace(1 + 11, (uchar)(((subPicture->getOfsX() & 0xf) << 4) | ((tmp >> 8) & 0xf)));
    controlHeader.replace(1 + 12, (uchar)(tmp&0xff));

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
    tmp = even.size() + odd.size() + 22 + (subPicture->isForced ? 1 : 0) + 4;
    controlHeader.replace(forcedOfs + 0, (uchar)((tmp >> 8) & 0xff));
    controlHeader.replace(forcedOfs + 1, (uchar)(tmp & 0xff));
    controlHeader.replace(1 + 24, (uchar)((tmp >> 8) & 0xff));
    controlHeader.replace(1 + 25, (uchar)(tmp & 0xff));

    // subpicture size
    tmp = even.size() + odd.size() + 4 + controlHeaderLen;
    headerFirst.replace(15, (uchar)(tmp >> 8));
    headerFirst.replace(16, (uchar)tmp);

    /* offset to control buffer - 2 is the size of the offset */
    tmp = even.size() + odd.size() + 2;
    headerFirst.replace(17, (uchar)(tmp >> 8));
    headerFirst.replace(18, (uchar)tmp);

    // unfortunately, the SUB format only allows 0x800 bytes to
    // be written per packet. If a packet is larger, it has to be
    // split into fragments <= 0x800 bytes which follow one after the other.

    int sizeRLE = even.size() + odd.size();
    int bufSize = packHeader.size() + headerFirst.size() + controlHeaderLen + sizeRLE;
    int numAdditionalPackets = 0;
    if (bufSize > 0x800)
    {
        // determine how many additional headers we will need
        // considering that each additional header also adds to the size
        // due to its own headers
        numAdditionalPackets = 1;
        int remainingRLEsize = sizeRLE  - ((0x800 - packHeader.size()) - headerFirst.size()); // size - 0x7df
        while (remainingRLEsize > (((0x800 - packHeader.size()) - headerNext.size()) - controlHeaderLen))
        {
            remainingRLEsize -= ((0x800 - packHeader.size()) - headerNext.size());
            bufSize += packHeader.size() + headerNext.size();
            ++numAdditionalPackets;
        }
        // packet length of the 1st packet should be the maximum size
        tmp = (0x800 - packHeader.size()) - 6;
    }
    else
    {
        tmp = ((bufSize - packHeader.size()) - 6);
    }

    // allocate and fill buffer
    QVector<uchar> buf((1 + numAdditionalPackets) * 0x800);

    int stuffingBytes;
    int diff = buf.size() - bufSize;
    if (diff > 0 && diff < 6)
    {
        stuffingBytes = diff;
    }
    else
    {
        stuffingBytes = 0;
    }

    int ofs = 0;
    for (int i = 0; i < packHeader.size(); ++i)
    {
        buf.replace(ofs++, packHeader[i]);
    }

    // set packet length
    tmp += stuffingBytes;
    headerFirst.replace(4, (uchar)(tmp >> 8));
    headerFirst.replace(5, (uchar)tmp);

    // set pts length
    headerFirst.replace(8, (uchar)(5 + stuffingBytes));

    // write header and use pts for stuffing bytes (if needed)
    for (int i = 0; i < 14; ++i)
    {
        buf.replace(ofs++, headerFirst[i]);
    }
    for (int i = 0; i < stuffingBytes; ++i)
    {
        buf.replace(ofs++, (uchar)0xff);
    }
    for (int i = 14; i < headerFirst.size(); ++i)
    {
        buf.replace(ofs++, headerFirst[i]);
    }

    // write (first part of) RLE buffer
    tmp = sizeRLE;
    if (numAdditionalPackets > 0)
    {
        tmp = (((0x800 - packHeader.size()) - stuffingBytes) - headerFirst.size());
        if (tmp > sizeRLE) // can only happen in 1st buffer
        {
            tmp = sizeRLE;
        }
    }
    for (int i = 0; i < tmp; ++i)
    {
        if (i < even.size())
        {
            buf.replace(ofs++, even[i]);
        }
        else
        {
            buf.replace(ofs++, odd[i - even.size()]);
        }
    }
    int ofsRLE=tmp;

    // fill gap in first packet with (parts of) control header
    // only if the control header is split over two packets
    int controlHeaderWritten = 0;
    if (numAdditionalPackets == 1 && ofs < 0x800)
    {
        for (; ofs < 0x800; ofs++)
        {
            buf.replace(ofs, controlHeader[forcedOfs + (controlHeaderWritten++)]);
        }
    }

    // write additional packets
    for (int p = 0; p < numAdditionalPackets; ++p)
    {
        int rleSizeLeft;
        if (p == (numAdditionalPackets - 1))
        {
            // last loop
            rleSizeLeft = sizeRLE - ofsRLE;
            tmp = ((headerNext.size() + (controlHeaderLen - controlHeaderWritten)) + (sizeRLE - ofsRLE)) - 6;
        }
        else
        {
            tmp = (0x800 - packHeader.size()) - 6;
            rleSizeLeft = ((0x800 - packHeader.size()) - headerNext.size());
            // now, again, it could happen that the RLE buffer runs out before the last package
            if (rleSizeLeft > (sizeRLE - ofsRLE))
            {
                rleSizeLeft = sizeRLE - ofsRLE;
            }
        }
        // copy packet headers
        packHeader.replace(13, (uchar)(0xf8));
        for (int i = 0; i < packHeader.size(); ++i)
        {
            buf.replace(ofs++, packHeader[i]);
        }

        // set packet length
        headerNext.replace(4, (uchar)(tmp >> 8));
        headerNext.replace(5, (uchar)tmp);
        for (int i = 0; i < headerNext.size(); ++i)
        {
            buf.replace(ofs++, headerNext[i]);
        }

        // copy RLE buffer
        for (int i = ofsRLE; i < (ofsRLE + rleSizeLeft); ++i)
        {
            if (i < even.size())
            {
                buf.replace(ofs++, even[i]);
            }
            else
            {
                buf.replace(ofs++, odd[i - even.size()]);
            }
        }
        ofsRLE += rleSizeLeft;
        // fill possible gap in all but last package with (parts of) control header
        // only if the control header is split over two packets
        // this can only happen in the package before the last one though
        if (p != (numAdditionalPackets - 1))
        {
            for (; ofs < ((p + 2) * 0x800); ++ofs)
            {
                buf.replace(ofs, controlHeader[forcedOfs + (controlHeaderWritten++)]);
            }
        }
    }

    // write (rest of) control header
    for (int i = controlHeaderWritten; i < controlHeaderLen; ++i)
    {
        buf.replace(ofs++, controlHeader[forcedOfs + i]);
    }

    // fill rest of last packet with padding bytes
    diff = buf.size() - ofs;
    if (diff >= 6)
    {
        diff -= 6;
        buf.replace(ofs++, 0x00);
        buf.replace(ofs++, 0x00);
        buf.replace(ofs++, 0x01);
        buf.replace(ofs++, (uchar)0xbe);
        buf.replace(ofs++, (uchar)(diff >> 8));
        buf.replace(ofs++, (uchar)diff);
        for (; ofs < buf.size(); ++ofs)
        {
            buf.replace(ofs, (uchar)0xff);
        }
    } // else should never happen due to stuffing bytes

    return buf;
}

void SubDVD::readIdx(int idxToRead)
{
    QScopedPointer<QFile> idxFile(new QFile(idxFileName));
    if (!idxFile->open(QIODevice::ReadOnly | QIODevice::Text))
    {
        throw QString("Idx file '%1' can not be opened for reading.").arg(idxFileName);
    }

    QTextStream idxTextStream(idxFile.data());
    QString inString;
    qint64 temp;
    int langIdx = 0;
    bool ignore = false;
    bool langIdxMatched = false;

    while (!(inString = idxTextStream.readLine()).isNull())
    {
        inString = inString.trimmed();
        if (inString.size() < 1 || inString[0] == '#')
        {
            continue;
        }
        int position = inString.indexOf(':');
        if (position == -1 || (inString.size() - position) <= 1)
        {
            subtitleProcessor->printError(QString("Illegal key: %1\n").arg(inString));
            continue;
        }
        QString key(inString.left(position).trimmed().toLower());
        QString value(inString.mid(position + 1, inString.size() - 1).trimmed());
        QStringList keyValue;
        bool ok;

        // size (e.g. "size: 720x576")
        if (key == "size")
        {
            keyValue = value.split("x", QString::SkipEmptyParts);
            if (keyValue.isEmpty() || keyValue.size() <= 1)
            {
                throw QString("Illegal size: %1").arg(value);
            }
            temp = keyValue[0].toInt(&ok);
            temp = ok ? temp : -1;
            if (temp < 2)
            {
                throw QString("Illegal screen width: %1").arg(keyValue[0]);
            }
            screenWidth = temp;
            temp = keyValue[1].toInt(&ok);
            temp = ok ? temp : -1;
            if (temp < 2)
            {
                throw QString("Illegal screen height: %1").arg(keyValue[1]);
            }
            screenHeight = temp;
            continue;
        }

        // origin (e.g. "org: 0, 0")
        if (key == "org")
        {
            keyValue = value.split(",", QString::SkipEmptyParts);
            if (keyValue.isEmpty() || keyValue.size() <= 1)
            {
                throw QString("Illegal origin: %1").arg(value);
            }
            temp = keyValue[0].toInt(&ok);
            temp = ok ? temp : -1;
            if (temp < 0)
            {
                throw QString("Illegal x origin: %1").arg(keyValue[0]);
            }
            ofsXglob = temp;
            temp = keyValue[1].toInt(&ok);
            temp = ok ? temp : -1;
            if (temp < 0)
            {
                throw QString("Illegal y origin: %1").arg(keyValue[1]);
            }
            ofsYglob = temp;
            continue;
        }

        // scale (e.g. "scale: 100%, 100%")
        if (key == "scale")
        {
            continue;
        }

        // alpha (e.g. "alpha: 100%")
        if (key == "alpha")
        {
            continue;
        }

        // smoothing (e.g. "smooth: OFF")
        if (key == "smooth")
        {
            continue;
        }

        // fading (e.g. "fadein/out: 0, 0");
        if (key == "fadein/out")
        {
            continue;
        }

        // alignment (e.g. "align: OFF at LEFT TOP")
        if (key == "align")
        {
            continue;
        }

        // time offset (e.g. "time offset: 0")
        if (key == "time offset")
        {
            temp = value.toInt(&ok);
            temp = ok ? temp : -1;
            if (temp < 0)
            {
                bool ok;
                temp = (int) TimeUtil::timeStrToPTS(value, &ok);
            }
            if (!ok)
            {
                throw QString("Illegal time offset: %1").arg(QString::number(temp));
            }
            delayGlob = temp * 90; //ms -> 90khz
            continue;
        }

        // forced subs (e.g. "forced subs: OFF")
        if (key == "forced subs")
        {
            continue;
        }

        // palette (e.g. "palette: 000000, 828282, 828282, 828282, 828282, 828282, 828282, ffffff, 828282, bababa, 828282, 828282, 828282, 828282, 828282, 828282")
        if (key == "palette")
        {
            QStringList paletteValues = value.split(",");
            if (paletteValues.isEmpty() || paletteValues.size() < 1 || paletteValues.size() > 16)
            {
                throw QString("Illegal palette definition: %1").arg(value);
            }
            for (int i = 0; i < paletteValues.size(); ++i)
            {
                int color = -1;
                temp = paletteValues[i].trimmed().toInt(&ok, 16);
                temp = ok ? temp : -1;
                if (temp == -1)
                {
                    throw QString("Illegal palette entry: %1").arg(paletteValues[i]);
                }
                color = temp;
                srcPalette->setARGB(i, color);
            }
            continue;
        }

        // custom colors (e.g. "custom colors: OFF, tridx: 1000, colors: 000000, 444444, 888888, cccccc")
        if (key == "custom colors")
        {
            continue;
        }

        // language index (e.g. "langidx: 0")
        if (key == "langidx" && idxToRead == -1)
        {
            temp = value.toInt(&ok);
            temp = ok ? temp : -1;
            if (temp < 0)
            {
                throw QString("Illegal language idx: %1").arg(temp);
            }
            langIdx = temp;
            continue;
        }
        else if (key == "langidx" && idxToRead != -1)
        {
            langIdx = idxToRead;
        }

        // language id (e.g. "id: de, index: 0")
        if (key == "id")
        {
            QString id;
            QStringList vals = value.split(',', QString::SkipEmptyParts);
            id = vals[0];
            if (id.size() != 2)
            {
                subtitleProcessor->printWarning(QString("Illegal language id: %1\n").arg(id));
                continue;
            }

            bool languageFound = false;
            auto subLanguages = subtitleProcessor->getLanguages();

            QString longLanguageName;
            for (int i = 0; i < subLanguages.size(); ++i)
            {
                if (id == "iw")
                {
                    subtitleProcessor->printWarning("IDX file uses deprecated language code: 'iw'. Changing to 'he'.\n");
                    id = "he";

                }
                if (id == "ji")
                {
                    subtitleProcessor->printWarning("IDX file uses deprecated language code: 'ji'. Changing to 'yi'.\n");
                    id = "yi";
                }
                if (id == subLanguages[i][1])
                {
                    longLanguageName = subLanguages[i][0];
                    languageFound = true;
                    break;
                }
            }
            if (!languageFound)
            {
                subtitleProcessor->printWarning(QString("Illegal language id: %1\n").arg(id));
            }
            vals = value.split(':', QString::SkipEmptyParts);
            if (vals.size() == 1)
            {
                subtitleProcessor->printError(QString("Missing index key: %1\n").arg(value));
                continue;
            }
            if (vals[0].toLower() == "index")
            {
                subtitleProcessor->printError(QString("Missing index key: %1\n").arg(vals[0]));
                continue;
            }
            temp = vals[1].toInt(&ok);
            temp = ok ? temp : -1;
            if (temp < 0)
            {
                throw QString("Illegal language index: %1").arg(temp);
            }
            if (temp != langIdx)
            {
                ignore = true;
                subtitleProcessor->printWarning(QString("Language id %1(index:%2) inactive -> ignored\n")
                                                .arg(id).arg(QString::number(temp)));
            }
            else
            {
                langIdxMatched = true;
                languageIdxRead = langIdx;
                for (int i = 0; i < subLanguages.size(); ++i)
                {
                    if (id == subLanguages[i][1])
                    {
                        languageIdx = i;
                    }
                }
                streamID = temp;
                ignore = false;
            }

            emit addLanguage(QString("%1 - %2").arg(QString::number(temp), 2, QChar('0')).arg(languageFound ? longLanguageName : "?"));
            continue;
        }

        if (!ignore)
        {
            // e.g. ("timestamp: 00:00:14:160, filepos: 000000000")
            if (key == "timestamp")
            {
                QString vs;
                QStringList vals = value.split(',', QString::SkipEmptyParts);
                if (vals.isEmpty() || vals.size() <= 1)
                {
                    throw QString("Illegal timestamp entry: %1").arg(value);
                }
                vs = vals[0];
                bool ok;
                long time = TimeUtil::timeStrToPTS(vs, &ok);
                if (!ok)
                {
                    throw QString("Illegal timestamp: %1").arg(vals[0]);
                }
                vs = vals[1].trimmed().toLower();
                vals = vs.split(':', QString::SkipEmptyParts);
                if (vals.isEmpty() || vals.size() <= 1)
                {
                    throw QString("Missing filepos: %1").arg(value);
                }
                long hex = vals[1].trimmed().toLong(&ok, 16);
                if (!ok)
                {
                    throw QString("Illegal filepos: %1").arg(vals[1]);
                }

                SubPictureDVD* pic = new SubPictureDVD;
                pic->offset = hex;
                pic->width = screenWidth;
                pic->height = screenHeight;
                pic->startTime = time + delayGlob;
                subPictures.push_back(pic);
            }
        }
    }

    if (!langIdxMatched)
    {
        throw QString("langidx: %1 does not match an index in the IDX file. "
                      "Unable to read any subpictures.").arg(QString::number(langIdx));
    }

    if (subPictures.size() == 0)
    {
        throw QString("No subpictures were detected in IDX file.");
    }

    emit maxProgressChanged(subPictures.size());
}

void SubDVD::writeIdx(QString filename, SubPicture *subPicture, QVector<int> offsets, QVector<int> timestamps, Palette *palette)
{
    QScopedPointer<QFile> out(new QFile(filename));
    if (!out->open(QIODevice::WriteOnly | QIODevice::Text))
    {
        throw QString("File: '%1' can not be opened for writing.").arg(filename);
    }

    out->write("# VobSub index file, v7 (do not modify this line!)\n");
    out->write(QString("# Created by " + progNameVer + "\n").toAscii());
    out->write("\n");
    out->write("# Frame size\n");
    out->write(QString("size: " + QString::number(subPicture->width) + "x" + QString::number((subPicture->height - (2 * subtitleProcessor->getCropOfsY()))) + "\n").toAscii());
    out->write("\n");
    out->write("# Origin - upper-left corner\n");
    out->write("org: 0, 0\n");
    out->write("\n");
    out->write("# Scaling\n");
    out->write("scale: 100%, 100%\n");
    out->write("\n");
    out->write("# Alpha blending\n");
    out->write("alpha: 100%\n");
    out->write("\n");
    out->write("# Smoothing\n");
    out->write("smooth: OFF\n");
    out->write("\n");
    out->write("# Fade in/out in milliseconds\n");
    out->write("fadein/out: 0, 0\n");
    out->write("\n");
    out->write("# Force subtitle placement relative to (org.x, org.y)\n");
    out->write("align: OFF at LEFT TOP\n");
    out->write("\n");
    out->write("# For correcting non-progressive desync. (in millisecs or hh:mm:ss:ms)\n");
    out->write("time offset: 0\n");
    out->write("\n");
    out->write("# ON: displays only forced subtitles, OFF: shows everything\n");
    out->write("forced subs: OFF\n");
    out->write("\n");
    out->write("# The palette of the generated file\n");
    out->write("palette: ");
    //Palette pal = Core.getCurrentDVDPalette();
    for (int i = 0; i < palette->getSize(); ++i)
    {
        QRgb val = palette->getRGB(i);
        QString value = QString("%1").arg(QString::number(val, 16), 6, QChar('0'));
        out->write(value.toAscii());
        if (i != palette->getSize() - 1)
        {
            out->write(", ");
        }
    }
    out->write("\n");
    out->write("\n");
    out->write("# Custom colors (transp idxs and the four colors)\n");
    out->write("custom colors: OFF, tridx: 1000, colors: 000000, 444444, 888888, cccccc\n");
    out->write("\n");
    out->write("# Language index in use\n");
    out->write("langidx: 0\n");
    out->write("\n");
    out->write(QString("# " + subtitleProcessor->getLanguages()[subtitleProcessor->getLanguageIdx()][0] + "\n").toAscii());
    out->write(QString("id: " + subtitleProcessor->getLanguages()[subtitleProcessor->getLanguageIdx()][1] + ", index: 0\n").toAscii());
    out->write("# Decomment next line to activate alternative name in DirectVobSub / Windows Media Player 6.x\n");
    out->write(QString("# alt: " + subtitleProcessor->getLanguages()[subtitleProcessor->getLanguageIdx()][0] + "\n").toAscii());
    out->write("# Vob/Cell ID: 1, 1 (PTS: 0)\n");
    for (int i = 0; i < timestamps.size(); ++i)
    {
        out->write(QString("timestamp: " + TimeUtil::ptsToTimeStrIdx(timestamps[i])).toAscii());
        QString value = QString("%1").arg(QString::number(offsets[i], 16), 9, QChar('0'));
        out->write(", filepos: " +  value.toAscii());
        out->write("\n");
    }
}
