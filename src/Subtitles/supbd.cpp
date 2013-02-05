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

#include "supbd.h"

#include "subtitleprocessor.h"
#include "Tools/filebuffer.h"
#include "Tools/timeutil.h"
#include "subpicturebd.h"
#include "imageobject.h"
#include "imageobjectfragment.h"
#include "bitmap.h"
#include "types.h"
#include "Tools/quantizefilter.h"
#include "Tools/numberutil.h"
#include "palette.h"
#include "paletteinfo.h"

#include <QImage>
#include <QPainter>

SupBD::SupBD(QString fileName, SubtitleProcessor* subtitleProcessor) :
    supFileName(fileName)
{
    this->subtitleProcessor = subtitleProcessor;
}

SupBD::~SupBD()
{
    if (!fileBuffer.isNull())
    {
        fileBuffer.reset();
    }
}

QImage SupBD::getImage()
{
    return bitmap.image(palette);
}

QImage SupBD::getImage(Bitmap &bitmap)
{
    return bitmap.image(palette);
}

void SupBD::decode(int index)
{
    if (index == 32)
    {
        int breakPoint = 0;
    }
    if (index < subPictures.size())
    {
        decode(&subPictures[index]);
    }
    else
    {
        throw QString("Index: %1 out of bounds.\n").arg(QString::number(index));
    }
}

int SupBD::getNumFrames()
{
    return subPictures.size();
}

bool SupBD::isForced(int index)
{
    return subPictures[index].isForced();
}

qint64 SupBD::getEndTime(int index)
{
    return subPictures[index].endTime();
}

qint64 SupBD::getStartTime(int index)
{
    return subPictures[index].startTime();
}

qint64 SupBD::getStartOffset(int index)
{
    return subPictures[index].getImgObj().getFragmentList()[0].imageBufferOffset();
}

SubPicture *SupBD::getSubPicture(int index)
{
    return &subPictures[index];
}

void SupBD::readAllSupFrames()
{
    fileBuffer.reset(new FileBuffer(supFileName));

    int index = 0;
    int bufsize = (int)fileBuffer->getSize();
    SupSegment segment;
    SubPictureBD pic;
    int compNum = -1;
    int subCount = 0;

    try
    {
        while (index < bufsize)
        {
            if (subtitleProcessor->isCancelled())
            {
                throw QString("Cancelled by user!");
            }
            emit currentProgressChanged(index);

            segment = readSegment(index);
            QString out;
            QString so; // hack to return string
            switch (segment.type)
            {
            case 0x14:
            {
                out = QString("PDS ofs:0x%1, size:0x%2").arg(QString::number(index, 16), 8, QChar('0'))
                                                        .arg(QString::number(segment.size, 16), 4, QChar('0'));

                if (pic.startTime() != -1)
                {
                    so = QString("");
                    int ps = parsePDS(&segment, &pic, so);

                    if (ps >= 0)
                    {
                        subtitleProcessor->print(QString("%1, %2\n").arg(out).arg(so));
                    }
                    else
                    {
                        subtitleProcessor->print(QString(out + "\n"));
                        subtitleProcessor->printWarning(QString(so + "\n"));
                    }
                }
                else
                {
                    subtitleProcessor->print(out + "\n");
                    subtitleProcessor->printWarning(QString("missing PTS start -> ignored\n"));
                }
            } break;
            case 0x15:
            {
                out = QString("ODS ofs:0x%1, size:0x%2").arg(QString::number(index, 16), 8, QChar('0'))
                                                        .arg(QString::number(segment.size, 16), 4, QChar('0'));

                if (pic.startTime() != -1)
                {
                    so = QString("");
                    parseODS(&segment, &pic, so);

                    if (!pic.paletteUpdated())
                    {
                        if (!so.isNull())
                        {
                            out += ", " + so;
                        }

                        subtitleProcessor->print(QString("%1, img size: %2*%3\n")
                                                 .arg(out)
                                                 .arg(QString::number(pic.getImageWidth()))
                                                 .arg(QString::number(pic.getImageHeight())));
                    }
                    else
                    {
                        subtitleProcessor->print(QString(out + "\n"));
                        subtitleProcessor->printWarning(QString("palette update only -> ignored\n"));
                    }
                }
                else
                {
                    subtitleProcessor->print(QString(out + "\n"));
                    subtitleProcessor->printWarning(QString("missing PTS start -> ignored\n"));
                }
            } break;
            case 0x16:
            {
                if (pic.startTime() != -1)
                {
                    if (pic.imageObjectList.size() != 0)
                    {
                        subtitleProcessor->printX(QString("#> %1 (%2)\n")
                                                  .arg(QString::number(++subCount))
                                                  .arg(TimeUtil::ptsToTimeStr(pic.startTime())));
                    }
                    subPictures.push_back(pic);
                    pic = SubPictureBD();
                }

                so = QString("");
                parsePCS(&segment, &pic, so);
                compNum = pic.compNum();

                out = QString("PCS ofs:0x%1, START, size:0x%2, comp#: %3, forced: %4")
                                            .arg(QString::number(index, 16), 8, QChar('0'))
                                            .arg(QString::number(segment.size, 16), 4, QChar('0'))
                                            .arg(QString::number(compNum))
                                            .arg(pic.isForced() ? "true" : "false");
                if (!so.isEmpty())
                {
                    out += ", " + so + "\n";
                }
                else
                {
                    out += "\n";
                }
                out += QString("PTS start: %1").arg(TimeUtil::ptsToTimeStr(pic.startTime()));
                out += QString(", screen size: %1*%2\n").arg(QString::number(pic.width())).arg(QString::number(pic.height()));
            } break;
            case 0x17:
            {
                out = QString("WDS ofs:0x%1, size:0x%2").arg(QString::number(index, 16), 8, QChar('0'))
                                                    .arg(QString::number(segment.size, 16), 4, QChar('0'));

                if (pic.startTime() != -1)
                {
                    parseWDS(&segment, &pic);

                    subtitleProcessor->print(QString("%1, dim: %2*%3\n")
                                             .arg(out)
                                             .arg(QString::number(pic.windowWidth()))
                                             .arg(QString::number(pic.windowHeight())));
                }
                else
                {
                    subtitleProcessor->print(QString(out + "\n"));
                    subtitleProcessor->printWarning(QString("Missing PTS start -> ignored\n"));
                }
            } break;
            case 0x80:
            {
                subtitleProcessor->print(QString("END ofs: 0x%1\n").arg(QString::number(index, 16), 8, QChar('0')));

                if (pic.startTime() != -1)
                {
                    if (pic.imageObjectList.size() != 0)
                    {
                        subtitleProcessor->printX(QString("#< %1 (%2)\n")
                                                  .arg(QString::number(++subCount))
                                                  .arg(TimeUtil::ptsToTimeStr(pic.startTime())));
                    }

                    subPictures.push_back(pic);
                    pic = SubPictureBD();
                }
            } break;
            default:
            {
                subtitleProcessor->printWarning(QString("<unknown> 0x%1 ofs: 0x%2\n")
                                                .arg(QString::number(segment.type, 16), 2, QChar('0'))
                                                .arg(QString::number(index, 16), 8, QChar('0')));
            } break;
            }
            index += 13; // header size
            index += segment.size;
        }
    }
    catch (QString e)
    {
        if (subPictures.size() == 0)
        {
            throw e;
        }
        subtitleProcessor->printError(QString(e + "\n"));
        subtitleProcessor->print(QString("Probably not all caption imported due to error.\n"));
        return;
    }

    if (pic.startTime() != -1)
    {
        subPictures.push_back(pic);
        subtitleProcessor->printX(QString("#> %1 (%2)\n")
                                  .arg(QString::number(subPictures.size()))
                                  .arg(TimeUtil::ptsToTimeStr(pic.startTime())));

        pic = SubPictureBD();
    }

    for (int i = 1; i < subPictures.size(); ++i)
    {
        subPictures[i - 1].setEndTime(subPictures[i].startTime());
    }

    for (int i = subPictures.size() - 1; i >= 0; --i)
    {
        // Need to remove any subpictures who have no image objects
        // or remove any duped subtitles that share a composition number
        if (subPictures[i].imageObjectList.size() == 0 || (i > 0 && (subPictures[i].compNum() == subPictures[i - 1].compNum())))
        {
            subPictures.remove(i);
        }
    }

    emit currentProgressChanged(bufsize);
    // count forced frames
    numForcedFrames = 0;
    for (auto subPicture : subPictures)
    {
        if (subPicture.isForced())
        {
            numForcedFrames++;
        }
    }

    subtitleProcessor->printX(QString("\nDetected %1 forced captions.\n")
                              .arg(QString::number(numForcedFrames)));
}

QVector<uchar> SupBD::encodeImage(Bitmap &bm)
{
    QVector<uchar> bytes;
    uchar color;
    int ofs;
    int len;

    for (int y = 0; y < bm.height(); ++y)
    {
        uchar* pixels = bm.image().scanLine(y);
        ofs = 0;
        int x;
        for (x = 0; x < bm.width(); x += len, ofs += len)
        {
            color = pixels[ofs];
            for (len = 1; (x + len) < bm.width(); ++len)
            {
                if (pixels[ofs + len] != color)
                {
                    break;
                }
            }
            if (len <= 2 && color != 0)
            {
                // only a single occurrence -> add color
                bytes.push_back(color);
                if (len == 2)
                {
                    bytes.push_back(color);
                }
            }
            else
            {
                if (len > 0x3fff)
                {
                    len = 0x3fff;
                }
                bytes.push_back(0); // rle id
                if (color == 0 && len < 0x40)
                {
                    // 00 xx -> xx times 0
                    bytes.push_back(len);
                }
                else if (color == 0)
                {
                    // 00 4x xx -> xxx zeroes
                    bytes.push_back(0x40 | (len >> 8));
                    bytes.push_back(len);
                }
                else if(len < 0x40)
                {
                    // 00 8x cc -> x times value cc
                    bytes.push_back(0x80 | len);
                    bytes.push_back(color);
                }
                else
                {
                    // 00 cx yy cc -> xyy times value cc
                    bytes.push_back(0xc0 | (len >> 8));
                    bytes.push_back(len);
                    bytes.push_back(color);
                }
            }
        }
        if (x == bm.width())
        {
            bytes.push_back(0); // rle id
            bytes.push_back(0);
        }
    }
    return bytes;
}

int SupBD::getFpsId(double fps)
{
    if (fps == FPS_24HZ)
    {
        return 0x20;
    }
    if (fps == FPS_PAL)
    {
        return 0x30;
    }
    if (fps == FPS_NTSC)
    {
        return 0x40;
    }
    if (fps == FPS_PAL_I)
    {
        return 0x60;
    }
    if (fps == FPS_NTSC_I)
    {
        return 0x70;
    }
    // assume FPS_24P (also for FPS_23_975)
    return 0x10;
}

QVector<uchar> SupBD::createSupFrame(SubPicture *subPicture, Bitmap &bm, Palette &pal)
{
    // the last palette entry must be transparent
    if (pal.size() > 255 && pal.alpha(255) > 0)
    {
        // quantize image
        QuantizeFilter qf;
        Bitmap bmQ(bm.width(), bm.height());
        QVector<QRgb> ct = qf.quantize(bm.toARGB(pal), &bmQ.image(), bm.width(), bm.height(), 255, false, false);
        int size = ct.size();

        subtitleProcessor->print(QString("Palette had to be reduced from %1 to %2 entries.\n")
                                 .arg(QString::number(pal.size()))
                                 .arg(QString::number(size)));

        if (size > 255)
        {
            size = 255;

            subtitleProcessor->printWarning(QString("Quantizer failed.\n"));
        }
        // create palette
        pal = new Palette(size);
        for (int i = 0; i < size; ++i)
        {
            pal.setARGB(i, ct[i]);
        }
        // use new bitmap
        bm = bmQ;
    }

    QVector<uchar> rleBuf = encodeImage(bm);

    // for some obscure reason, a packet can be a maximum 0xfffc bytes
    // since 13 bytes are needed for the header("PG", PTS, DTS, ID, SIZE)
    // there are only 0xffef bytes available for the packet
    // since the first ODS packet needs an additional 11 bytes for info
    // and the following ODS packets need 4 additional bytes, the
    // first package can store only 0xffe4 RLE buffer bytes and the
    // following packets can store 0xffeb RLE buffer bytes
    int numAddPackets;
    if (rleBuf.size() <= 0xffe4)
    {
        numAddPackets = 0; // no additional packets needed;
    }
    else
    {
        numAddPackets = 1 + ((rleBuf.size() - 0xffe4) / 0xffeb);
    }

    // a typical frame consists of 8 packets. It can be enlonged by additional
    // object frames
    int palSize = bm.highestColorIndex(pal) + 1;
    int size = packetHeader.size() * (8 + numAddPackets);
    size += headerPCSStart.size() + headerPCSEnd.size();
    size += (2 * headerWDS.size()) + headerODSFirst.size();
    size += numAddPackets * headerODSNext.size();
    size += 2 + (palSize * 5) /* PDS */;
    size += rleBuf.size();

    int yOfs = subPicture->getOfsY() - subtitleProcessor->getCropOfsY();
    if (yOfs < 0)
    {
        yOfs = 0;
    }
    else
    {
        int yMax = (subPicture->height() - subPicture->getImageHeight()) - (2 * subtitleProcessor->getCropOfsY());
        if (yOfs > yMax)
        {
            yOfs = yMax;
        }
    }

    int h = subPicture->height() - (2 * subtitleProcessor->getCropOfsY());

    QVector<uchar> buf(size);
    int index = 0;

    int fpsId = getFpsId(subtitleProcessor->getFPSTrg());

    /* time (in 90kHz resolution) needed to initialize (clear) the screen buffer
       based on the composition pixel rate of 256e6 bit/s - always rounded up */
    int frameInitTime = (((subPicture->width() * subPicture->height()) * 9) + 3199) / 3200; // better use default height here
    /* time (in 90kHz resolution) needed to initialize (clear) the window area
       based on the composition pixel rate of 256e6 bit/s - always rounded up
       Note: no cropping etc. -> window size == image size */
    int windowInitTime = (((bm.width() * bm.height()) * 9) + 3199) / 3200;
    /* time (in 90kHz resolution) needed to decode the image
       based on the decoding pixel rate of 128e6 bit/s - always rounded up  */
    int imageDecodeTime = (((bm.width() * bm.height()) * 9) + 1599) / 1600;
    // write PCS start
    packetHeader.replace(10, 0x16);                                             // ID
    NumberUtil::setDWord(packetHeader, 2, (int)subPicture->startTime());				// PTS
    NumberUtil::setDWord(packetHeader, 6, 0);                                           // DTS (0)
    NumberUtil::setWord(packetHeader, 11, headerPCSStart.size());			// size
    for (int i = 0; i < packetHeader.size(); ++i)
    {
        buf.replace(index++, packetHeader[i]);
    }
    NumberUtil::setWord(headerPCSStart, 0, subPicture->width());
    NumberUtil::setWord(headerPCSStart, 2, h);								// cropped height
    NumberUtil::setByte(headerPCSStart, 4, fpsId);
    NumberUtil::setWord(headerPCSStart, 5, subPicture->compNum());
    headerPCSStart.replace(14, (subPicture->isForced() ? (uchar)0x40 : 0));
    NumberUtil::setWord(headerPCSStart, 15, subPicture->getOfsX());
    NumberUtil::setWord(headerPCSStart, 17, yOfs);
    for (int i = 0; i < headerPCSStart.size(); ++i)
    {
        buf.replace(index++, headerPCSStart[i]);
    }

    // write WDS
    packetHeader.replace(10, 0x17);											// ID
    int timeStamp = (int)subPicture->startTime() - windowInitTime;
    NumberUtil::setDWord(packetHeader, 2, timeStamp);						// PTS
    NumberUtil::setDWord(packetHeader, 6, 0);                               //DTS (0)
    NumberUtil::setWord(packetHeader, 11, headerWDS.size());				// size
    for (int i = 0; i < packetHeader.size(); ++i)
    {
        buf.replace(index++, packetHeader[i]);
    }
    NumberUtil::setWord(headerWDS, 2, subPicture->getOfsX());
    NumberUtil::setWord(headerWDS, 4, yOfs);
    NumberUtil::setWord(headerWDS, 6, bm.width());
    NumberUtil::setWord(headerWDS, 8, bm.height());
    for (int i = 0; i < headerWDS.size(); ++i)
    {
        buf.replace(index++, headerWDS[i]);
    }

    // write PDS
    packetHeader.replace(10, 0x14);											// ID
    int dts = (int)subPicture->startTime() - (frameInitTime + windowInitTime);
    NumberUtil::setDWord(packetHeader, 2, dts);								// PTS
    NumberUtil::setDWord(packetHeader, 6, 0);								// DTS (0)
    NumberUtil::setWord(packetHeader, 11, (2 + (palSize * 5)));					// size
    for (int i = 0; i < packetHeader.size(); ++i)
    {
        buf.replace(index++, packetHeader[i]);
    }
    buf.replace(index++, 0);
    buf.replace(index++, 0);
    for (int i = 0; i < palSize; ++i)
    {
        buf.replace(index++, (uchar)i);											// index
        buf.replace(index++, pal.Y()[i]);									// Y
        buf.replace(index++, pal.Cr()[i]);									// Cr
        buf.replace(index++, pal.Cb()[i]);									// Cb
        buf.replace(index++, qAlpha(pal.colorTable()[i]));   				// Alpha
    }

    // write first OBJ
    int bufSize = rleBuf.size();
    int rleIndex = 0;
    if (bufSize > 0xffe4)
    {
        bufSize = 0xffe4;
    }
    packetHeader.replace(10, 0x15);											// ID
    timeStamp = dts + imageDecodeTime;
    NumberUtil::setDWord(packetHeader, 2, timeStamp);						// PTS
    NumberUtil::setDWord(packetHeader, 6, 0);								// DTS (0)
    NumberUtil::setWord(packetHeader, 11, headerODSFirst.size() + bufSize);	// size
    for (int i = 0; i < packetHeader.size(); ++i)
    {
        buf.replace(index++, packetHeader[i]);
    }
    int marker = ((numAddPackets == 0) ? 0xC0000000 : 0x80000000);
    NumberUtil::setDWord(headerODSFirst, 3, marker | (rleBuf.size() + 4));
    NumberUtil::setWord(headerODSFirst, 7, bm.width());
    NumberUtil::setWord(headerODSFirst, 9, bm.height());
    for (int i = 0; i < headerODSFirst.size(); ++i)
    {
        buf.replace(index++, headerODSFirst[i]);
    }
    for (int i=0; i<bufSize; i++)
    {
        buf.replace(index++, rleBuf[rleIndex++]);
    }

    // write additional OBJ packets
    bufSize = rleBuf.size() - bufSize; // remaining bytes to write
    for (int p = 0; p < numAddPackets; ++p)
    {
        int psize = bufSize;
        if (psize > 0xffeb)
        {
            psize = 0xffeb;
        }
        packetHeader.replace(10, 0x15);										// ID (keep DTS & PTS)
        NumberUtil::setWord(packetHeader, 11, headerODSNext.size() + psize);	// size
        for (int i = 0; i < packetHeader.size(); ++i)
        {
            buf.replace(index++, packetHeader[i]);
        }
        for (int i = 0; i < headerODSNext.size(); ++i)
        {
            buf.replace(index++, headerODSNext[i]);
        }
        for (int i = 0; i < psize; ++i)
        {
            buf.replace(index++, rleBuf[rleIndex++]);
        }
        bufSize -= psize;
    }

    // write END
    packetHeader.replace(10, 0x80);										// ID
    NumberUtil::setDWord(packetHeader, 2, timeStamp);    					// PTS
    NumberUtil::setDWord(packetHeader, 6, 0);								// DTS (0)
    NumberUtil::setWord(packetHeader, 11, 0);								// size
    for (int i = 0; i < packetHeader.size(); ++i)
    {
        buf.replace(index++, packetHeader[i]);
    }

    // write PCS end
    packetHeader.replace(10, 0x16);											// ID
    NumberUtil::setDWord(packetHeader, 2, (int)subPicture->endTime());				// PTS
    NumberUtil::setDWord(packetHeader, 6, 0);								// DTS (0)
    NumberUtil::setWord(packetHeader, 11, headerPCSEnd.size());				// size
    for (int i = 0; i < packetHeader.size(); ++i)
    {
        buf.replace(index++, packetHeader[i]);
    }
    NumberUtil::setWord(headerPCSEnd, 0, subPicture->width());
    NumberUtil::setWord(headerPCSEnd, 2, h);									// cropped height
    NumberUtil::setByte(headerPCSEnd, 4, fpsId);
    NumberUtil::setWord(headerPCSEnd, 5, subPicture->compNum() + 1);
    for (int i = 0; i < headerPCSEnd.size(); ++i)
    {
        buf.replace(index++, headerPCSEnd[i]);
    }

    // write WDS
    packetHeader.replace(10, 0x17);											// ID
    timeStamp = (int)subPicture->endTime() - windowInitTime;
    NumberUtil::setDWord(packetHeader, 2, timeStamp);						// PTS
    NumberUtil::setDWord(packetHeader, 6, 0);                               // DTS (0)
    NumberUtil::setWord(packetHeader, 11, headerWDS.size());				// size
    for (int i = 0; i < packetHeader.size(); ++i)
    {
        buf.replace(index++, packetHeader[i]);
    }
    NumberUtil::setWord(headerWDS, 2, subPicture->getOfsX());
    NumberUtil::setWord(headerWDS, 4, yOfs);
    NumberUtil::setWord(headerWDS, 6, bm.width());
    NumberUtil::setWord(headerWDS, 8, bm.height());
    for (int i = 0; i < headerWDS.size(); ++i)
    {
        buf.replace(index++, headerWDS[i]);
    }

    // write END
    packetHeader.replace(10, 0x80);										// ID
    NumberUtil::setDWord(packetHeader, 2, timeStamp);						// PTS (PTS of end PCS)
    NumberUtil::setDWord(packetHeader, 6, 0);								// DTS (0)
    NumberUtil::setWord(packetHeader, 11, 0);								// size
    for (int i = 0; i < packetHeader.size(); ++i)
    {
        buf.replace(index++, packetHeader[i]);
    }

    return buf;
}


double SupBD::getFps(int index)
{
    return getFpsFromID(subPictures[index].subPictureType());
}

SupBD::SupSegment SupBD::readSegment(int offset)
{
    SupSegment segment;
    if (fileBuffer->getWord(offset) != 0x5047)
    {
        throw QString("PG missing at index 0x").arg(QString::number(offset, 16), 8, QChar('0'));
    }
    segment.timestamp = fileBuffer->getDWord(offset += 2); // read PTS
    offset += 4; /* ignore DTS */
    segment.type = fileBuffer->getByte(offset += 4);
    segment.size = fileBuffer->getWord(++offset);
    segment.offset = offset + 2;
    return segment;
}

bool SupBD::picMergable(SubPictureBD *a, SubPictureBD *b)
{
    bool eq = false;
    if (a != 0 && b != 0)
    {
        if (a->endTime() == 0 || (b->startTime() - a->endTime()) < subtitleProcessor->getMergePTSdiff())
        {
            ImageObject ao = a->getImgObj();
            ImageObject bo = b->getImgObj();
            if ((ao.bufferSize() == bo.bufferSize())
                    && (ao.width() == bo.width())
                    && (ao.height() == bo.height()))
            {
                eq = true;
            }
        }
    }
    return eq;
}

void SupBD::parsePCS(SupSegment *segment, SubPictureBD *subPicture, QString &msg)
{
    int index = segment->offset;

    if (segment->size >= 4)
    {
        subPicture->setStartTime(segment->timestamp);
        subPicture->setWidth(fileBuffer->getWord(index));            // video_width
        subPicture->setHeight(fileBuffer->getWord(index + 2));        // video_height
        int type = fileBuffer->getByte(index + 4);                  // hi nibble: frame_rate, lo nibble: reserved
        int num  = fileBuffer->getWord(index + 5);                  // composition_number
        // 8bit  composition_state: 0x00: normal, 		0x40: acquisition point
        //							0x80: epoch start,	0xC0: epoch continue, 6bit reserved
        CompositionState cs = getCompositionState(segment);
        // 8bit  palette_update_flag (0x80), 7bit reserved
        subPicture->setPaletteUpdated(fileBuffer->getByte(index + 8) == 0x080);
        int palID = fileBuffer->getByte(index + 9);                 // 8bit  palette_id_ref
        int coNum = fileBuffer->getByte(index + 10);                // 8bit  number_of_composition_objects (0..2)


        if (cs == CompositionState::INVALID)
        {
            subtitleProcessor->printWarning(QString("Illegal composition state at offset 0x%1\n")
                                            .arg(QString::number(index, 16), 8, QChar('0')));
        }
        else
        {
            int offset = index;
            for (int i = 0; i < coNum; ++i)
            {
                // composition_object:
                int objID = fileBuffer->getWord(offset + 11);            // 16bit object_id_ref
                msg = QString("palID: %1, objID: %2").arg(QString::number(palID)).arg(QString::number(objID));
                ImageObject imgObj;

                if (objID >= subPicture->imageObjectList.size())
                {
                    imgObj = ImageObject();
                }
                else
                {
                    imgObj = subPicture->getImgObj(objID);
                }
                imgObj.setPaletteID(palID);
                subPicture->setObjectID(objID);
                subPicture->setPaletteID(palID);

                // skipped:  8bit  window_id_ref
                if (segment->size >= 0x13)
                {
                    subPicture->setSubPictureType(type);
                    // object_cropped_flag: 0x80, forced_on_flag = 0x040, 6bit reserved
                    int forcedCropped = fileBuffer->getByte(offset + 14);
                    subPicture->setCompNum(num);
                    subPicture->setForced((forcedCropped & 0x40) == 0x40);
                    // composition_object_horizontal_position
                    imgObj.setXOffset(fileBuffer->getWord(offset + 15));
                    // composition_object_vertical_position
                    imgObj.setYOffset(fileBuffer->getWord(offset + 17));
                }

                if (objID >= subPicture->imageObjectList.size())
                {
                    subPicture->imageObjectList.resize(objID + 1);
                    subPicture->imageObjectList.insert(objID, imgObj);
                }
                else
                {
                    subPicture->imageObjectList.replace(objID, imgObj);
                }
                offset += 8;
            }
        }
    }
}

int SupBD::parsePDS(SupSegment *segment, SubPictureBD *subPicture, QString &msg)
{
    int index = segment->offset;
    int paletteID = fileBuffer->getByte(index);	// 8bit palette ID (0..7)
    // 8bit palette version number (incremented for each palette change)
    int paletteUpdate = fileBuffer->getByte(index + 1);
    int paletteSize = (segment->size - 2) / 5;

    if (paletteSize <= 0)
    {
        return 0;
    }

    subPicture->setPaletteID(paletteID);

    if (subPicture->palettes.isEmpty())
    {
        subPicture->palettes = QVector<QVector<PaletteInfo> >(8);
    }
    if (paletteID > 7)
    {
        msg = QString("Illegal palette id at offset 0x%1").arg(QString::number(index, 16), 8, QChar('0'));
        return -1;
    }
    //
    QVector<PaletteInfo> al = subPicture->palettes[paletteID];
    PaletteInfo p;
    p.setPaletteSize(paletteSize);
    p.setPaletteOffset(index + 2);
    al.push_back(p);
    subPicture->palettes.replace(paletteID, al);

    msg = QString("ID: %1, update: %2, %3 entries")
            .arg(QString::number(paletteID))
            .arg(QString::number(paletteUpdate))
            .arg(QString::number(p.paletteSize()));

    return paletteSize;
}

bool SupBD::parseODS(SupSegment *segment, SubPictureBD *subPicture, QString &msg)
{
    int index = segment->offset;

    ImageObjectFragment info;
    int objID = fileBuffer->getWord(index);         // 16bit object_id
    int objVer = fileBuffer->getByte(index+1);		// 16bit object_id
    int objSeq = fileBuffer->getByte(index+3);		// 8bit  first_in_sequence (0x80),
                                                    // last_in_sequence (0x40), 6bits reserved

    bool first = ((objSeq & 0x80) == 0x80);
    bool last  = ((objSeq & 0x40) == 0x40);

    ImageObject imgObj;

    if (objID >= subPicture->imageObjectList.size())
    {
        imgObj = ImageObject();
    }
    else
    {
        imgObj = subPicture->getImgObj(objID);
    }

    if (imgObj.getFragmentList().isEmpty() || first)       // 8bit  object_version_number
    {
        // skipped:
        //  24bit object_data_length - full RLE buffer length (including 4 bytes size info)
        int width  = fileBuffer->getWord(index + 7);  // object_width
        int height = fileBuffer->getWord(index + 9);  // object_height

        if (width <= subPicture->width() && height <= subPicture->height())
        {
            imgObj.getFragmentList().clear();
            info = ImageObjectFragment();
            info.setImageBufferOffset(index + 11);
            info.setImagePacketSize(segment->size - ((index + 11) - segment->offset));

            imgObj.getFragmentList().push_back(info);
            imgObj.setBufferSize(info.imagePacketSize());
            imgObj.setHeight(height);
            imgObj.setWidth(width);

            if (objID >= subPicture->imageObjectList.size())
            {
                subPicture->imageObjectList.push_back(imgObj);
            }
            else
            {
                subPicture->imageObjectList.replace(objID, imgObj);
            }

            msg = QString("ID: %1, update: %2, seq: %3%4%5").arg(QString::number(objID))
                                                            .arg(QString::number(objVer))
                                                            .arg(first ? QString("first") : QString(""))
                                                            .arg((first && last) ? QString("/") : QString(""))
                                                            .arg(last ? QString("last") : QString(""));

            return true;
        }
        else
        {
            subtitleProcessor->printWarning(QString("Invalid image size - ignored\n"));
            return false;
        }
    }
    else
    {
        // object_data_fragment
        // skipped:
        //  16bit object_id
        //  8bit  object_version_number
        //  8bit  first_in_sequence (0x80), last_in_sequence (0x40), 6bits reserved
        info = ImageObjectFragment();
        info.setImageBufferOffset(index + 4);
        info.setImagePacketSize(segment->size - ((index + 4) - segment->offset));

        imgObj.getFragmentList().push_back(info);
        imgObj.setBufferSize(imgObj.bufferSize() + info.imagePacketSize());

        msg = QString("ID: %1, update: %2, seq: %3%4%5").arg(QString::number(objID))
                                                        .arg(QString::number(objVer))
                                                        .arg(first ? QString("first") : QString(""))
                                                        .arg((first && last) ? QString("/") : QString(""))
                                                        .arg(last ? QString("last") : QString(""));

        if (objID >= subPicture->imageObjectList.size())
        {
            subPicture->imageObjectList.push_back(imgObj);
        }
        else
        {
            subPicture->imageObjectList.replace(objID, imgObj);
        }

        return false;
    }
}

void SupBD::parseWDS(SupSegment* segment, SubPictureBD *subPicture)
{
    int index = segment->offset;

    if (segment->size >= 10)
    {
        // skipped:
        // 8bit: number of windows (currently assumed 1, 0..2 is legal)
        // 8bit: window id (0..1)
        subPicture->setXWindowOffset(fileBuffer->getWord(index + 2));       // window_horizontal_position
        subPicture->setYWindowOffset(fileBuffer->getWord(index + 4));       // window_vertical_position
        subPicture->setWindowWidth(fileBuffer->getWord(index + 6));      // window_width
        subPicture->setWindowHeight(fileBuffer->getWord(index + 8));     // window_height
    }
}

SupBD::CompositionState SupBD::getCompositionState(SupBD::SupSegment *segment)
{
    int type;
    type = fileBuffer->getByte(segment->offset + 7);
    switch (type)
    {
    case 0x00:
    {
        return CompositionState::NORMAL;
    }
    case 0x40:
    {
        return CompositionState::ACQU_POINT;
    }
    case 0x80:
    {
        return CompositionState::EPOCH_START;
    }
    case 0xC0:
    {
        return CompositionState::EPOCH_CONTINUE;
    }
    default:
    {
        return CompositionState::INVALID;
    }
    }
}

void SupBD::findImageArea(SubPictureBD *subPicture)
{
    QImage image = bitmap.image(palette);
    int top = 0, bottom = 0, left = 0, right = 0;
    const QRgb* colors = image.colorTable().constData();

    const uchar *pixels = image.constScanLine(0);
    int pitch = image.bytesPerLine();
    QRgb color = palette.rgba(primaryColorIndex);

    for (int i = 0; i < image.height(); ++i)
    {
        for (int j = 0; j < image.width(); ++j)
        {
            if (colors[pixels[j]] == color)
            {
                top = i - 20; // Pad value for borders
                if (top < 0)
                {
                    top = 0;
                }
                goto find_bottom;
            }
        }
        pixels += pitch;
    }

find_bottom:
    pixels = image.constScanLine(image.height() - 1);

    for (int i = image.height() - 1; i >= 0; --i)
    {
        for (int j = 0; j < image.width(); ++j)
        {
            if (colors[pixels[j]] == color)
            {
                bottom = i + 20; // Pad value for borders
                if (bottom >= image.height())
                {
                    bottom = image.height() - 1;
                }
                goto find_left;
            }
        }
        pixels -= pitch;
    }

find_left:
    pixels = image.constScanLine(top);
    int tempLeft = image.width();

    for (int i = top; i <= bottom; ++i)
    {
        for (int j = 0; j < image.width(); ++j)
        {
            if (colors[pixels[j]] == color)
            {
                if (j < tempLeft)
                {
                    tempLeft = j - 20; // Pad value for borders
                }
                if (tempLeft < 0)
                {
                    tempLeft = 0;
                }
            }
        }
        pixels += pitch;
    }
    left = tempLeft;

    pixels = image.constScanLine(top);
    int tempRight = 0;

    for (int i = top; i <= bottom; ++i)
    {
        for (int j = image.width() - 1; j >= 0; --j)
        {
            if (colors[pixels[j]] == color)
            {
                if (j > tempRight)
                {
                    tempRight = j + 20; // Pad value for borders
                }
                if (tempRight >= image.width())
                {
                    tempRight = image.width() - 1;
                }
            }
        }
        pixels += pitch;
    }
    right = tempRight;

    int height = (bottom - top) + 1;
    int width = (right - left) + 1;

    subPicture->setXWindowOffset(left);
    subPicture->setYWindowOffset(top);
    subPicture->setOfsX(left);
    subPicture->setOfsY(top);

    subPicture->setWindowHeight(height);
    subPicture->setWindowWidth(width);

    subPicture->setImageHeight(height);
    subPicture->setImageWidth(width);

    QImage newImage = image.copy(left, top, width, height);
    bitmap.setImg(newImage);
}

void SupBD::decode(SubPictureBD *subPicture)
{
    palette = decodePalette(subPicture);
    bitmap = decodeImage(subPicture, palette.transparentIndex());
    primaryColorIndex = bitmap.primaryColorIndex(palette, subtitleProcessor->getAlphaThreshold());

    // special case for subtitles that don't properly signal the image area
    if (subPicture->getImgObj().width() == 1920 && subPicture->getImgObj().height() == 1080)
    {
        findImageArea(subPicture);
    }
}

Palette SupBD::decodePalette(SubPictureBD *subPicture)
{
    bool fadeOut = false;
    int palIndex = 0;
    if (subPicture->imageObjectList.size() > 1)
    {
        int breakPoint = 0;
    }
    int paletteId = subPicture->getImgObj().paletteID();
    QVector<PaletteInfo> pl = subPicture->palettes[paletteId];
    if (pl.isEmpty())
    {
        throw QString("Palette ID out of bounds.");
    }

    Palette palette(256, subtitleProcessor->usesBT601());
    // by definition, index 0xff is always completely transparent
    // also all entries must be fully transparent after initialization

    for (int j = 0; j < pl.size(); j++)
    {
        PaletteInfo p = pl[j];
        int index = p.paletteOffset();
        for (int i = 0; i < p.paletteSize(); ++i)
        {
            // each palette entry consists of 5 bytes
            palIndex = fileBuffer->getByte(index);
            int y = fileBuffer->getByte(++index);
            int cr, cb;
            if (subtitleProcessor->getSwapCrCb())
            {
                cb = fileBuffer->getByte(++index);
                cr = fileBuffer->getByte(++index);
            }
            else
            {
                cr = fileBuffer->getByte(++index);
                cb = fileBuffer->getByte(++index);
            }
            int alpha = fileBuffer->getByte(++index);

            int alphaOld = palette.alpha(palIndex);
            // avoid fading out
            if (alpha >= alphaOld)
            {
                // to not mess with scaling algorithms, make transparent color black
                if (alpha < subtitleProcessor->getAlphaCrop())
                {
                    y = 16;
                    cr = 128;
                    cb = 128;
                }
                palette.setAlpha(palIndex, alpha);
            }
            else
            {
                fadeOut = true;
            }

            palette.setYCbCr(palIndex, y, cb, cr);
            index++;
        }
    }
    if (fadeOut)
    {
        subtitleProcessor->printWarning(QString("fade out detected -> patched palette\n"));
    }
    subPicture->palettes.replace(paletteId, pl);
    return palette;
}

Bitmap SupBD::decodeImage(SubPictureBD *subPicture, int transparentIndex)
{
    int numImgObj = 0;
    QVector<int> objectIdxes;

    for (int i = 0; i < subPicture->imageObjectList.size(); ++i)
    {
        if (subPicture->imageObjectList[i].bufferSize() > 0)
        {
            ++numImgObj;
            objectIdxes.push_back(i);
        }
    }

    QVector<Bitmap> bitmaps;

    for (int i = 0; i < objectIdxes.size(); ++i)
    {
        int imgObjIdx = objectIdxes[i];
        ImageObject imageObjet = subPicture->getImgObj(imgObjIdx);
        int w = imageObjet.width();
        int h = imageObjet.height();
        // always decode image obj 0, start with first entry in fragmentlist
        ImageObjectFragment fragment = imageObjet.getFragmentList()[0];
        qint64 startOfs = fragment.imageBufferOffset();

        if (w > subPicture->width() || h > subPicture->height())
        {
            throw QString("Subpicture too large: %1x%2 at offset 0x%3")
                    .arg(QString::number(w))
                    .arg(QString::number(h))
                    .arg(QString::number(startOfs, 16), 8, QChar('0'));
        }

        Bitmap bm(w, h, transparentIndex);

        int b;
        int index = 0;
        int ofs = 0;
        int size = 0;
        int xpos = 0;

        // just for multi-packet support, copy all of the image data in one common buffer
        QVector<uchar> buf = QVector<uchar>(imageObjet.bufferSize());
        index = 0;
        for (int p = 0; p < imageObjet.getFragmentList().size(); ++p)
        {
            // copy data of all packet to one common buffer
            fragment = imageObjet.getFragmentList()[p];
            for (int i = 0; i < fragment.imagePacketSize(); ++i)
            {
                buf[index + i] = (uchar)fileBuffer->getByte(fragment.imageBufferOffset() + i);
            }
            index += fragment.imagePacketSize();
        }

        index = 0;

        uchar* pixels = bm.image().bits();
        int pitch = bm.image().bytesPerLine();

        do
        {
            b = buf[index++] & 0xff;
            if (b == 0)
            {
                b = buf[index++] & 0xff;
                if (b == 0)
                {
                    // next line
                    ofs = (ofs / pitch) * pitch;
                    if (xpos < pitch)
                    {
                        ofs += pitch;
                    }
                    xpos = 0;
                }
                else
                {
                    if ((b & 0xC0) == 0x40)
                    {
                        // 00 4x xx -> xxx zeroes
                        size = ((b - 0x40) << 8) + (buf[index++] & 0xff);

                        for (int i = 0; i < size; ++i)
                        {
                            pixels[ofs++] = 0; /*(uchar)b;*/
                        }
                        xpos += size;
                    }
                    else if ((b & 0xC0) == 0x80)
                    {
                        // 00 8x yy -> x times value y
                        size = (b - 0x80);
                        b = buf[index++] & 0xff;

                        for (int i = 0; i < size; ++i)
                        {
                            pixels[ofs++] = (uchar)b;
                        }
                        xpos += size;
                    }
                    else if  ((b & 0xC0) != 0)
                    {
                        // 00 cx yy zz -> xyy times value z
                        size = ((b - 0xC0) << 8)+(buf[index++] & 0xff);
                        b = buf[index++] & 0xff;

                        for (int i = 0; i < size; ++i)
                        {
                            pixels[ofs++] = (uchar)b;
                        }
                        xpos += size;
                    }
                    else
                    {
                        // 00 xx -> xx times 0
                        for (int i = 0; i < b; ++i)
                        {
                            pixels[ofs++] = 0;
                        }
                        xpos += b;
                    }
                }
            }
            else
            {
                pixels[ofs++] = (uchar)b;
                xpos++;
            }
        } while (index < buf.size());
        bitmaps.push_back(bm);
    }

    if (bitmaps.size() == 1)
    {
        ImageObject imageObject = subPicture->getImgObj();

        subPicture->setOfsX(imageObject.xOffset());
        subPicture->setOfsY(imageObject.yOffset());
        subPicture->setImageWidth(imageObject.width());
        subPicture->setImageHeight(imageObject.height());

        return bitmaps[0];
    }
    else
    {
        ImageObject imgObj1 = subPicture->getImgObj(objectIdxes[0]);
        ImageObject imgObj2 = subPicture->getImgObj(objectIdxes[1]);

        int imgObj1XOfs = imgObj1.xOffset();
        int imgObj2XOfs = imgObj2.xOffset();
        int imgObj1YOfs = imgObj1.yOffset();
        int imgObj2YOfs = imgObj2.yOffset();

        int resultXOffset = imgObj1XOfs < imgObj2XOfs ? imgObj1XOfs : imgObj2XOfs;
        int resultYOffset = imgObj1YOfs < imgObj2YOfs ? imgObj1YOfs : imgObj2YOfs;

        int resultHeight = 0;

        if (imgObj1YOfs > imgObj2YOfs)
        {
            resultHeight = ((imgObj1YOfs + imgObj1.height()) - resultYOffset);
        }
        else
        {
            resultHeight = ((imgObj2YOfs + imgObj2.height()) - resultYOffset);
        }

        int imgObj1Width = imgObj1.width();
        int imgObj2Width = imgObj2.width();

        int resultWidth = imgObj1Width > imgObj2Width ? imgObj1Width : imgObj2Width;

        QImage resultImage(1920, 1080, QImage::Format_ARGB32_Premultiplied);
        QPainter painter(&resultImage);
        painter.setCompositionMode(QPainter::CompositionMode_Source);
        painter.fillRect(resultImage.rect(), Qt::transparent);
        painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
        painter.drawImage(imgObj1XOfs, imgObj1YOfs, bitmaps[0].image(palette));
        painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
        painter.drawImage(imgObj2XOfs, imgObj2YOfs, bitmaps[1].image(palette));
        painter.end();

        resultImage = resultImage.convertToFormat(QImage::Format_Indexed8, palette.colorTable());
        resultImage = resultImage.copy(resultXOffset, resultYOffset, resultWidth, resultHeight);

        subPicture->setOfsX(resultXOffset);
        subPicture->setOfsY(resultYOffset);
        subPicture->setImageWidth(resultWidth);
        subPicture->setImageHeight(resultHeight);

        return Bitmap(resultImage);
    }
}

double SupBD::getFpsFromID(int id)
{
    switch (id)
    {
    case 0x20:
        return FPS_24HZ;
    case 0x30:
        return FPS_PAL;
    case 0x40:
        return FPS_NTSC;
    case 0x60:
        return FPS_PAL_I;
    case 0x70:
        return FPS_NTSC_I;
    default:
        return FPS_24P; // assume FPS_24P (also for FPS_23_975)
    }
}
