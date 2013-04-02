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
#include "bitmap.h"
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

QImage SupBD::image()
{
    return _bitmap.image(_palette);
}

QImage SupBD::image(Bitmap &bitmap)
{
    return bitmap.image(_palette);
}

void SupBD::decode(int index)
{
    if (index < subPictures.size())
    {
        decode(&subPictures[index]);
    }
    else
    {
        throw QString("Index: %1 out of bounds.\n").arg(QString::number(index));
    }
}

int SupBD::numFrames()
{
    return subPictures.size();
}

bool SupBD::isForced(int index)
{
    return subPictures[index].isForced();
}

qint64 SupBD::endTime(int index)
{
    return subPictures[index].endTime();
}

qint64 SupBD::startTime(int index)
{
    return subPictures[index].startTime();
}

qint64 SupBD::startOffset(int index)
{
    // return offset of first imageobject with image fragments
    for (int i = 0; i < subPictures[index].imageObjectList.size(); ++i)
    {
        if (subPictures[index].imageObjectList[i].fragmentList().size() > 0)
        {
            return subPictures[index].imageObjectList[i].fragmentList()[0].imageBufferOffset();
        }
    }
    return 0;
}

SubPicture *SupBD::subPicture(int index)
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
    QMap<int, QVector<PaletteInfo>> palettes;
    QMap<int, QVector<ODS>> imageObjects;
    int subCount = 0;
    bool forceFirstOds = true;
    ODS ods;
    WDS wds;
    PCS pcs;
    PDS pds;

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
                if (pcs.pts != -1)
                {
                    out = QString("PDS ofs:0x%1, size:0x%2").arg(QString::number(index, 16), 8, QChar('0'))
                                                            .arg(QString::number(segment.size, 16), 4, QChar('0'));

                    so = QString("");
                    if (pcs.pts == 3007080)
                    {
                        int breakPoint = 1;
                    }
                    pds = parsePDS(&segment, so);

                    if (pds.paletteSize >= 0)
                    {
                        subtitleProcessor->print(QString("%1, %2\n").arg(out).arg(so));
                    }
                    else
                    {
                        subtitleProcessor->print(QString(out + "\n"));
                        subtitleProcessor->printWarning(QString(so + "\n"));
                    }

                    int palId = pds.paletteId;
                    if (pds.paletteInfo.paletteSize() != -1)
                    {
                        if (!palettes.contains(pds.paletteId))
                        {
                            palettes[palId] = QVector<PaletteInfo>();
                        }
                        else
                        {
                            if (pic.paletteUpdated())
                            {
                                palettes[palId].remove(palettes[palId].size() - 1);
                            }
                        }
                        palettes[palId].push_back(pds.paletteInfo);
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
                if (pcs.pts != -1)
                {
                    out = QString("ODS ofs:0x%1, size:0x%2").arg(QString::number(index, 16), 8, QChar('0'))
                                                            .arg(QString::number(segment.size, 16), 4, QChar('0'));

                    so = QString("");
                    bool isFirst = true;
                    ods = parseODS(&segment, so, forceFirstOds, isFirst);

                    if (!pcs.paletteUpdate)
                    {
                        if (isFirst)
                        {
                            imageObjects[ods.objectId] = QVector<ODS> { ods };
                            subtitleProcessor->print(QString("%1, img size: %2*%3\n")
                                                     .arg(out)
                                                     .arg(QString::number(ods.width))
                                                     .arg(QString::number(ods.height)));
                        }
                        else
                        {
                            imageObjects[ods.objectId].push_back(ods);
                        }
                        if (!so.isNull())
                        {
                            out += ", " + so;
                        }
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
                forceFirstOds = false;
            } break;
            case 0x16:
            {
                if (pcs.pts != -1)
                {
                    if (pic.numCompObjects() != 0)
                    {
                        subtitleProcessor->printX(QString("#> %1 (%2)\n")
                                                  .arg(QString::number(++subCount))
                                                  .arg(TimeUtil::ptsToTimeStr(pcs.pts)));
                    }

                    pic.setData(pcs, imageObjects, palettes, wds);

                    subPictures.push_back(pic);
                    pic = SubPictureBD();
                    pcs = PCS();
                }

                forceFirstOds = true;

                so = QString("");
                pcs = parsePCS(&segment, so);

                out = QString("PCS ofs:0x%1, START, size:0x%2, comp#: %3, forced: %4")
                                            .arg(QString::number(index, 16), 8, QChar('0'))
                                            .arg(QString::number(segment.size, 16), 4, QChar('0'))
                                            .arg(QString::number(pcs.compositionNumber))
                                            .arg(pic.isForced() ? "true" : "false");

                if (pcs.compositionState == CompositionState::EPOCH_START)
                {
                    palettes.clear();
                    imageObjects.clear();
                }

                if (!so.isEmpty())
                {
                    out += ", " + so + "\n";
                }
                else
                {
                    out += "\n";
                }
                out += QString("PTS start: %1").arg(TimeUtil::ptsToTimeStr(pcs.pts));
                out += QString(", screen size: %1*%2\n").arg(QString::number(pcs.videoWidth)).arg(QString::number(pcs.videoHeight));
            } break;
            case 0x17:
            {
                out = QString("WDS ofs:0x%1, size:0x%2").arg(QString::number(index, 16), 8, QChar('0'))
                                                    .arg(QString::number(segment.size, 16), 4, QChar('0'));

                if (pcs.pts != -1)
                {
                    wds = parseWDS(&segment);

                    if (wds.numberOfWindows > 0)
                    {
                        subtitleProcessor->print(QString("%1").arg(out));

                        for (int i = 0; i < wds.numberOfWindows; ++i)
                        {
                            subtitleProcessor->print(QString(", windows %1 dim: %2*%3\n")
                                                     .arg(QString::number(i))
                                                     .arg(QString::number(wds.windows[i].width()))
                                                     .arg(QString::number(wds.windows[i].height())));
                        }
                    }
                }
                else
                {
                    subtitleProcessor->print(QString(out + "\n"));
                    subtitleProcessor->printWarning(QString("Missing PTS start -> ignored\n"));
                }
            } break;
            case 0x80:
            {
                forceFirstOds = true;
                subtitleProcessor->print(QString("END ofs: 0x%1\n").arg(QString::number(index, 16), 8, QChar('0')));

                if (pcs.pts != -1)
                {
                    pic.setData(pcs, imageObjects, palettes, wds);

                    if (pic.numCompObjects() != 0)
                    {
                        subtitleProcessor->printX(QString("#< %1 (%2)\n")
                                                  .arg(QString::number(++subCount))
                                                  .arg(TimeUtil::ptsToTimeStr(pcs.pts)));
                    }

                    subPictures.push_back(pic);
                    pic = SubPictureBD();
                    pcs = PCS();
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

    if (pcs.pts != -1)
    {
        pic.setData(pcs, imageObjects, palettes, wds);

        if (pic.numCompObjects() != 0)
        {
            subtitleProcessor->printX(QString("#< %1 (%2)\n")
                                      .arg(QString::number(++subCount))
                                      .arg(TimeUtil::ptsToTimeStr(pcs.pts)));
        }

        subPictures.push_back(pic);
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
        if (subPictures[i].numCompObjects() == 0)
        {
            subPictures.remove(i);
        }
    }

    emit currentProgressChanged(bufsize);
    // count forced frames
    _numForcedFrames = 0;
    for (auto subPicture : subPictures)
    {
        if (subPicture.isForced())
        {
            _numForcedFrames++;
        }
    }

    subtitleProcessor->printX(QString("\nDetected %1 forced captions.\n")
                              .arg(QString::number(_numForcedFrames)));
}

QVector<uchar> SupBD::encodeImage(Bitmap &bm)
{
    uchar color;
    int ofs;
    int len;

    int width = bm.width();
    int height = bm.height();


    const uchar* pixels = bm.image().constScanLine(0);
    int sourcePitch = bm.image().bytesPerLine();

    QVector<uchar> bytes;
    bytes.reserve(height * sourcePitch);

    for (int y = 0; y < height; ++y)
    {
        ofs = 0;
        int x;
        for (x = 0; x < width; x += len, ofs += len)
        {
            color = pixels[ofs];
            for (len = 1; (x + len) < width && pixels[ofs + len] == color && len <= 0x3fff; ++len)
            {
            }

            if (len < 3 && color)
            {
                // only a single occurrence -> add color
                bytes.push_back(color);
                if (len == 2)
                {
                    bytes.push_back(color);
                }
                continue;
            }

            bytes.push_back(0); // rle id

            if (!color)
            {
                if (len >= 0x40)
                {
                    // 00 4x xx -> xxx zeroes
                    bytes.push_back(0x40 | (len >> 8));
                }
                // 00 xx -> xx times 0
                bytes.push_back(len);
            }
            else
            {
                if (len < 0x40)
                {
                    // 00 8x cc -> x times value cc
                    bytes.push_back(0x80 | len);
                }
                else
                {
                    // 00 cx yy cc -> xyy times value cc
                    bytes.push_back(0xc0 | (len >> 8));
                    bytes.push_back(len);
                }
                bytes.push_back(color);
            }
        }

        bytes.push_back(0); // rle id
        bytes.push_back(0);

        pixels += sourcePitch;
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

    int yOfs = subPicture->y() - subtitleProcessor->getCropOfsY();
    if (yOfs < 0)
    {
        yOfs = 0;
    }
    else
    {
        int yMax = (subPicture->screenHeight() - subPicture->imageHeight()) - (2 * subtitleProcessor->getCropOfsY());
        if (subPicture->windowSizes().size() == 1 && yOfs > yMax)
        {
            yOfs = yMax;
        }
    }

    QVector<QVector<uchar>> rleBuf;
    int rleBufSize = 0;

    QMap<int, QRect> imageSizes;
    QMap<int, QRect> windows;
    int numberOfWindows = 1, numberOfImageObjects = 1;

    if (subPicture->numCompObjects() >= 1)
    {
        int i = 0;
        for (int objectId : subPicture->objectIDs())
        {
            imageSizes[i] = subPicture->imageSizes()[objectId];
            windows[i] = subPicture->windowSizes()[objectId];
            ++i;
        }
        numberOfImageObjects = subPicture->numCompObjects();
    }
    else
    {
        imageSizes[0] = QRect(subPicture->x(), yOfs, bm.width(), bm.height());
    }

    if (subPicture->windowSizes().size() > 1)
    {
        numberOfWindows = subPicture->numberOfWindows();
    }

    QVector<Bitmap> bitmaps;
    int xOffset1 = 0, xOffset2 = 0, yOffset1 = 0, yOffset2 = 0;

    if (numberOfImageObjects == 1)
    {
        bitmaps.push_back(bm);
    }
    else
    {
        if (imageSizes[0].x() < imageSizes[1].x())
        {
            if (imageSizes[0].y() < imageSizes[1].y())
            {
                xOffset2 = imageSizes[1].x() - imageSizes[0].x();
                yOffset2 = imageSizes[1].y() - imageSizes[0].y();
            }
            else
            {
                yOffset1 = imageSizes[0].y() - imageSizes[1].y();
                xOffset2 = imageSizes[1].x() - imageSizes[0].x();
            }
        }
        else
        {
            if (imageSizes[0].y() < imageSizes[1].y())
            {
                xOffset1 = imageSizes[0].x() - imageSizes[1].x();
                yOffset2 = imageSizes[1].y() - imageSizes[0].y();
            }
            else
            {
                xOffset1 = imageSizes[0].x() - imageSizes[1].x();
                yOffset1 = imageSizes[0].y() - imageSizes[1].y();
            }
        }

        bitmaps.push_back(bm.crop(xOffset1, yOffset1, imageSizes[0].width(), imageSizes[0].height()));
        bitmaps.push_back(bm.crop(xOffset2, yOffset2, imageSizes[1].width(), imageSizes[1].height()));
    }

    for (int i = 0; i < bitmaps.size(); ++i)
    {
        QVector<uchar> buf = encodeImage(bitmaps[i]);
        rleBufSize += buf.size();
        rleBuf.push_back(buf);
    }

    // for some obscure reason, a packet can be a maximum 0xfffc bytes
    // since 13 bytes are needed for the header("PG", PTS, DTS, ID, SIZE)
    // there are only 0xffef bytes available for the packet
    // since the first ODS packet needs an additional 11 bytes for info
    // and the following ODS packets need 4 additional bytes, the
    // first package can store only 0xffe4 RLE buffer bytes and the
    // following packets can store 0xffeb RLE buffer bytes
    int numAddPackets = 0;
    for (int i = 0; i < rleBuf.size(); ++i)
    {
        if (rleBuf[i].size() > 0xffe4)
        {
            numAddPackets += 1 + ((rleBuf[i].size() - 0xffe4) / 0xffeb);
        }
    }

    // a typical frame consists of 8 packets. It can be enlonged by additional
    // object frames
    int palSize = bm.highestColorIndex(pal) + 1;
    int numPacketHeaders = (8 + numAddPackets + (numberOfImageObjects - 1));
    int numPCSNext = numberOfImageObjects - 1;
    int numPCSEnd = 1;
    int numWDS = 2;
    int numWDSNext = (1 + (numberOfWindows - 1)) * (numberOfWindows - 1);
    int size = packetHeader.size() * numPacketHeaders;
    size += headerPCSStart.size() + (numPCSNext * headerPCSNext.size()) + (numPCSEnd * headerPCSEnd.size());
    size += (numWDS * headerWDS.size()) + (numWDSNext * headerWDSNext.size());
    size += (numberOfImageObjects * headerODSFirst.size()) + (numAddPackets * headerODSNext.size());
    size += 2 + (palSize * 5) /* PDS */;
    size += rleBufSize;

    int h = subPicture->screenHeight() - (2 * subtitleProcessor->getCropOfsY());

    QVector<uchar> buf(size);
    int index = 0;

    int fpsId = getFpsId(subtitleProcessor->getFPSTrg());

    /* time (in 90kHz resolution) needed to initialize (clear) the screen buffer
       based on the composition pixel rate of 256e6 bit/s - always rounded up */
    int frameInitTime = (((subPicture->screenWidth() * subPicture->screenHeight()) * 9) + 3199) / 3200; // better use default height here
    /* time (in 90kHz resolution) needed to initialize (clear) the window area
       based on the composition pixel rate of 256e6 bit/s - always rounded up
       Note: no cropping etc. -> window size == image size */
    int windowInitTime = (((bm.width() * bm.height()) * 9) + 3199) / 3200;
    /* time (in 90kHz resolution) needed to decode the image
       based on the decoding pixel rate of 128e6 bit/s - always rounded up  */
    int imageDecodeTime = (((bm.width() * bm.height()) * 9) + 1599) / 1600;
    // write PCS start
    packetHeader.replace(10, 0x16);                                                 // ID
    NumberUtil::setDWord(packetHeader, 2, (int)subPicture->startTime());            // PTS
    NumberUtil::setDWord(packetHeader, 6, 0);                                       // DTS (0)
    NumberUtil::setWord(packetHeader, 11, headerPCSStart.size() +                   // size
                       ((numberOfImageObjects - 1) * headerPCSNext.size()));
    for (int i = 0; i < packetHeader.size(); ++i)
    {
        buf.replace(index++, packetHeader[i]);
    }
    NumberUtil::setWord(headerPCSStart, 0, subPicture->screenWidth());
    NumberUtil::setWord(headerPCSStart, 2, h);                                      // cropped height
    NumberUtil::setByte(headerPCSStart, 4, fpsId);
    NumberUtil::setWord(headerPCSStart, 5, subPicture->compNum());
    NumberUtil::setByte(headerPCSStart, 10, numberOfImageObjects);
    headerPCSStart.replace(14, (subPicture->isForced() ? (uchar)0x40 : 0));
    NumberUtil::setWord(headerPCSStart, 15, imageSizes[0].x());
    NumberUtil::setWord(headerPCSStart, 17, imageSizes[0].y());
    for (int i = 0; i < headerPCSStart.size(); ++i)
    {
        buf.replace(index++, headerPCSStart[i]);
    }
    if (numberOfImageObjects > 1)
    {
        headerPCSNext.replace(3, (subPicture->isForced() ? (uchar)0x40 : 0));
        NumberUtil::setWord(headerPCSNext, 4, imageSizes[1].x());
        NumberUtil::setWord(headerPCSNext, 6, imageSizes[1].y());
        for (int i = 0; i < headerPCSNext.size(); ++i)
        {
            buf.replace(index++, headerPCSNext[i]);
        }
    }

    // write WDS
    packetHeader.replace(10, 0x17);                                                 // ID
    int timeStamp = (int)subPicture->startTime() - windowInitTime;
    NumberUtil::setDWord(packetHeader, 2, timeStamp);                               // PTS
    NumberUtil::setDWord(packetHeader, 6, 0);                                       // DTS (0)
    NumberUtil::setWord(packetHeader, 11,                                           // size
                       (headerWDS.size() + ((numberOfWindows - 1) * headerWDSNext.size())));
    for (int i = 0; i < packetHeader.size(); ++i)
    {
        buf.replace(index++, packetHeader[i]);
    }
    NumberUtil::setByte(headerWDS, 0, numberOfWindows);
    NumberUtil::setWord(headerWDS, 2, windows[0].x());
    NumberUtil::setWord(headerWDS, 4, windows[0].y());
    NumberUtil::setWord(headerWDS, 6, windows[0].width());
    NumberUtil::setWord(headerWDS, 8, windows[0].height());
    for (int i = 0; i < headerWDS.size(); ++i)
    {
        buf.replace(index++, headerWDS[i]);
    }
    if (numberOfWindows > 1)
    {
        NumberUtil::setWord(headerWDSNext, 1, windows[1].x());
        NumberUtil::setWord(headerWDSNext, 3, windows[1].y());
        NumberUtil::setWord(headerWDSNext, 5, windows[1].width());
        NumberUtil::setWord(headerWDSNext, 7, windows[1].height());
        for (int i = 0; i < headerWDSNext.size(); ++i)
        {
            buf.replace(index++, headerWDSNext[i]);
        }
    }

    // write PDS
    packetHeader.replace(10, 0x14);                                                 // ID
    int dts = (int)subPicture->startTime() - (frameInitTime + windowInitTime);
    NumberUtil::setDWord(packetHeader, 2, dts);                                     // PTS
    NumberUtil::setDWord(packetHeader, 6, 0);                                       // DTS (0)
    NumberUtil::setWord(packetHeader, 11, (2 + (palSize * 5)));                     // size
    for (int i = 0; i < packetHeader.size(); ++i)
    {
        buf.replace(index++, packetHeader[i]);
    }
    buf.replace(index++, 0);
    buf.replace(index++, 0);
    for (int i = 0; i < palSize; ++i)
    {
        buf.replace(index++, (uchar)i);                                             // index
        buf.replace(index++, pal.Y()[i]);                                           // Y
        buf.replace(index++, pal.Cr()[i]);                                          // Cr
        buf.replace(index++, pal.Cb()[i]);                                          // Cb
        buf.replace(index++, qAlpha(pal.colorTable()[i]));                          // Alpha
    }

    for (int j = 0; j < numberOfImageObjects; ++j)
    {
        // write first OBJ
        int bufSize = rleBuf[j].size();
        int rleIndex = 0;
        if (bufSize > 0xffe4)
        {
            bufSize = 0xffe4;
        }
        packetHeader.replace(10, 0x15);                                             // ID
        timeStamp = dts + imageDecodeTime;
        NumberUtil::setDWord(packetHeader, 2, timeStamp);                           // PTS
        NumberUtil::setDWord(packetHeader, 6, 0);                                   // DTS (0)
        NumberUtil::setWord(packetHeader, 11, headerODSFirst.size() + bufSize);     // size
        for (int i = 0; i < packetHeader.size(); ++i)
        {
            buf.replace(index++, packetHeader[i]);
        }
        int marker = ((numAddPackets == 0) ? 0xC0000000 : 0x80000000);
        NumberUtil::setWord(headerODSFirst, 0, j);
        NumberUtil::setDWord(headerODSFirst, 3, marker | (rleBuf[j].size() + 4));
        NumberUtil::setWord(headerODSFirst, 7, bitmaps[j].width());
        NumberUtil::setWord(headerODSFirst, 9, bitmaps[j].height());
        for (int i = 0; i < headerODSFirst.size(); ++i)
        {
            buf.replace(index++, headerODSFirst[i]);
        }
        for (int i = 0; i < bufSize; ++i)
        {
            buf.replace(index++, rleBuf[j][rleIndex++]);
        }

        // write additional OBJ packets
        bufSize = rleBuf[j].size() - bufSize; // remaining bytes to write
        if (bufSize > 0)
        {
            int psize = bufSize;
            if (psize > 0xffeb)
            {
                psize = 0xffeb;
            }
            packetHeader.replace(10, 0x15);                                         // ID (keep DTS & PTS)
            NumberUtil::setWord(packetHeader, 11, headerODSNext.size() + psize);    // size
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
                buf.replace(index++, rleBuf[j][rleIndex++]);
            }
            bufSize -= psize;
        }
    }

    // write END
    packetHeader.replace(10, 0x80);                                                 // ID
    NumberUtil::setDWord(packetHeader, 2, timeStamp);                               // PTS
    NumberUtil::setDWord(packetHeader, 6, 0);                                       // DTS (0)
    NumberUtil::setWord(packetHeader, 11, 0);                                       // size
    for (int i = 0; i < packetHeader.size(); ++i)
    {
        buf.replace(index++, packetHeader[i]);
    }

    // write PCS end
    packetHeader.replace(10, 0x16);                                                 // ID
    NumberUtil::setDWord(packetHeader, 2, (int)subPicture->endTime());				// PTS
    NumberUtil::setDWord(packetHeader, 6, 0);                                       // DTS (0)
    NumberUtil::setWord(packetHeader, 11, headerPCSEnd.size());                     // size
    for (int i = 0; i < packetHeader.size(); ++i)
    {
        buf.replace(index++, packetHeader[i]);
    }
    NumberUtil::setWord(headerPCSEnd, 0, subPicture->screenWidth());
    NumberUtil::setWord(headerPCSEnd, 2, h);                                        // cropped height
    NumberUtil::setByte(headerPCSEnd, 4, fpsId);
    NumberUtil::setWord(headerPCSEnd, 5, subPicture->compNum() + 1);
    for (int i = 0; i < headerPCSEnd.size(); ++i)
    {
        buf.replace(index++, headerPCSEnd[i]);
    }

    // write WDS
    packetHeader.replace(10, 0x17);                                                 // ID
    timeStamp = (int)subPicture->endTime() - windowInitTime;
    NumberUtil::setDWord(packetHeader, 2, timeStamp);                               // PTS
    NumberUtil::setDWord(packetHeader, 6, 0);                                       // DTS (0)
    NumberUtil::setWord(packetHeader, 11,                                           // size
                       (headerWDS.size() + ((numberOfWindows - 1) * headerWDSNext.size())));
    for (int i = 0; i < packetHeader.size(); ++i)
    {
        buf.replace(index++, packetHeader[i]);
    }
    NumberUtil::setByte(headerWDS, 0, numberOfWindows);
    NumberUtil::setWord(headerWDS, 2, windows[0].x());
    NumberUtil::setWord(headerWDS, 4, windows[0].y());
    NumberUtil::setWord(headerWDS, 6, windows[0].width());
    NumberUtil::setWord(headerWDS, 8, windows[0].height());
    for (int i = 0; i < headerWDS.size(); ++i)
    {
        buf.replace(index++, headerWDS[i]);
    }
    if (numberOfWindows > 1)
    {
        NumberUtil::setWord(headerWDSNext, 1, windows[1].x());
        NumberUtil::setWord(headerWDSNext, 3, windows[1].y());
        NumberUtil::setWord(headerWDSNext, 5, windows[1].width());
        NumberUtil::setWord(headerWDSNext, 7, windows[1].height());
        for (int i = 0; i < headerWDSNext.size(); ++i)
        {
            buf.replace(index++, headerWDSNext[i]);
        }
    }

    // write END
    packetHeader.replace(10, 0x80);                                                 // ID
    NumberUtil::setDWord(packetHeader, 2, timeStamp);                               // PTS (PTS of end PCS)
    NumberUtil::setDWord(packetHeader, 6, 0);                                       // DTS (0)
    NumberUtil::setWord(packetHeader, 11, 0);                                       // size
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
    segment.timestamp = fileBuffer->getDWord(offset += 2);                          // read PTS
    offset += 4; /* ignore DTS */
    segment.type = fileBuffer->getByte(offset += 4);
    segment.size = fileBuffer->getWord(++offset);
    segment.offset = offset + 2;
    return segment;
}

PCS SupBD::parsePCS(SupSegment *segment, QString &msg)
{
    int index = segment->offset;

    PCS pcs;

    if (segment->size >= 4)
    {
        pcs.pts = segment->timestamp;
        pcs.videoWidth = fileBuffer->getWord(index);                                // video_width
        pcs.videoHeight = fileBuffer->getWord(index + 2);                           // video_height
        pcs.frameRate = fileBuffer->getByte(index + 4);                             // hi nibble: frame_rate, lo nibble: reserved
        pcs.compositionNumber = fileBuffer->getWord(index + 5);                     // composition_number

        // 8bit  composition_state: 0x00: normal, 0x40: acquisition point
        //							0x80: epoch start, 0xC0: epoch continue, 6bit reserved
        pcs.compositionState = getCompositionState(segment);
        pcs.paletteUpdate = (fileBuffer->getByte(index + 8) == 0x080);              // 8bit  palette_update_flag (0x80), 7bit reserved
        pcs.paletteId = fileBuffer->getByte(index + 9);                             // 8bit  palette_id_ref
        pcs.numberOfCompositionObjects = fileBuffer->getByte(index + 10);           // 8bit  number_of_composition_objects (0..2)

        if (pcs.compositionState == CompositionState::INVALID)
        {
            subtitleProcessor->printWarning(QString("Illegal composition state at offset 0x%1\n")
                                            .arg(QString::number(index, 16), 8, QChar('0')));
        }
        else
        {
            int offset = index;
            for (int i = 0; i < pcs.numberOfCompositionObjects; ++i)
            {
                // composition_object:
                int objectId = fileBuffer->getWord(offset + 11);
                pcs.objectIds.push_back(objectId);                                  // 16bit object_id_ref
                pcs.windowIds[objectId] = fileBuffer->getByte(offset + 13);         // 13: window_id_ref (0..1)

                msg = QString("palID: %1, objID: %2").arg(QString::number(pcs.paletteId)).arg(QString::number(objectId));

                pcs.forcedFlags[objectId] = fileBuffer->getByte(offset + 14);       // object_cropped_flag: 0x80, forced_on_flag = 0x040, 6bit reserved
                pcs.xPositions[objectId] = fileBuffer->getWord(offset + 15);
                pcs.yPositions[objectId] = fileBuffer->getWord(offset + 17);

                offset += 8;
            }
        }
    }
    return pcs;
}

PDS SupBD::parsePDS(SupSegment *segment, QString &msg)
{
    int index = segment->offset;

    PDS pds;
    // 8bit palette ID (0..7)
    pds.paletteId = fileBuffer->getByte(index);
    // 8bit palette version number (incremented for each palette change)
    pds.paletteVersion = fileBuffer->getByte(index + 1);
    pds.paletteSize = (segment->size - 2) / 5;

    if (pds.paletteSize <= 0)
    {
        return pds;
    }

    PaletteInfo paletteInfo;
    paletteInfo.setPaletteSize(pds.paletteSize);
    paletteInfo.setPaletteOffset(index + 2);

    pds.paletteInfo = paletteInfo;

    msg = QString("ID: %1, update: %2, %3 entries")
            .arg(QString::number(pds.paletteId))
            .arg(QString::number(pds.paletteVersion))
            .arg(QString::number(pds.paletteSize));

    return pds;
}

ODS SupBD::parseODS(SupSegment *segment, QString &msg, bool forceFirst, bool &isFirst)
{
    int index = segment->offset;

    int objID = fileBuffer->getWord(index);             // 16bit object_id
    int objVer = fileBuffer->getByte(index+1);          // 8bit object_version
    int objSeq = fileBuffer->getByte(index+3);          // 8bit  first_in_sequence (0x80),
                                                        // last_in_sequence (0x40), 6bits reserved

    isFirst = ((objSeq & 0x80) == 0x80) || forceFirst;
    bool last  = ((objSeq & 0x40) == 0x40);

    ODS ods;
    ImageObjectFragment info;
    if (isFirst)
    {
        ods.width = fileBuffer->getWord(index + 7);     // object_width
        ods.height = fileBuffer->getWord(index + 9);    // object_height

        info.setImageBufferOffset(index + 11);
        info.setImagePacketSize(segment->size - ((index + 11) - segment->offset));

        ods.fragment = info;
        ods.objectId = objID;
        ods.objectVersion = objVer;
        ods.objectSequence = objSeq;

        msg = QString("ID: %1, update: %2, seq: %3%4%5").arg(QString::number(objID))
                                                        .arg(QString::number(objVer))
                                                        .arg(isFirst ? QString("first") : QString(""))
                                                        .arg((isFirst && last) ? QString("/") : QString(""))
                                                        .arg(last ? QString("last") : QString(""));
    }
    else
    {
        // object_data_fragment
        info.setImageBufferOffset(index + 4);
        info.setImagePacketSize(segment->size - ((index + 4) - segment->offset));

        ods.fragment = info;
        ods.objectId = objID;
        ods.objectVersion = objVer;
        ods.objectSequence = objSeq;

        msg = QString("ID: %1, update: %2, seq: %3%4%5").arg(QString::number(objID))
                                                        .arg(QString::number(objVer))
                                                        .arg(isFirst ? QString("first") : QString(""))
                                                        .arg((isFirst && last) ? QString("/") : QString(""))
                                                        .arg(last ? QString("last") : QString(""));
    }
    return ods;
}

WDS SupBD::parseWDS(SupSegment* segment)
{
    int index = segment->offset;

    WDS wds;

    // 8bit: number of windows (0..2)
    wds.numberOfWindows = fileBuffer->getByte(index);

    for (int i = 0; i < wds.numberOfWindows; ++i)
    {
        int windowId = fileBuffer->getByte(index + 1);
        wds.windowIds.push_back(windowId);
        int xOfs = fileBuffer->getWord(index + 2);
        int yOfs = fileBuffer->getWord(index + 4);
        int width = fileBuffer->getWord(index + 6);
        int height = fileBuffer->getWord(index + 8);

        QRect rect(xOfs,                            // window_horizontal_position
                   yOfs,                            // window_vertical_position
                   width,                           // window_width
                   height);                         // window_height

        wds.windows[windowId] = rect;

        index += 9;                                 // Skip 9 bytes to read next window info
    }

    return wds;
}

CompositionState SupBD::getCompositionState(SupBD::SupSegment *segment)
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
    QImage image = _bitmap.image(_palette);
    int top = 0, bottom = 0, left = 0, right = 0;
    QVector<QRgb> colors = image.colorTable();//.constData();

    const uchar *pixels = image.constScanLine(0);
    int pitch = image.bytesPerLine();
    QRgb color = _palette.rgba(_primaryColorIndex);

    int imageHeight = image.height();
    int imageWidth = image.width();

    image.save("C:/temp/subs/beforeCrop.bmp");

    for (int i = 0; i < imageHeight; ++i)
    {
        for (int j = 0; j < imageWidth; ++j)
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
    pixels = image.constScanLine(imageHeight - 1);

    for (int i = imageHeight - 1; i >= 0; --i)
    {
        for (int j = 0; j < imageWidth; ++j)
        {
            if (colors[pixels[j]] == color)
            {
                bottom = i + 20; // Pad value for borders
                if (bottom >= imageHeight)
                {
                    bottom = imageHeight - 1;
                }
                goto find_left;
            }
        }
        pixels -= pitch;
    }

find_left:
    pixels = image.constScanLine(top);
    int tempLeft = imageWidth;

    for (int i = top; i <= bottom; ++i)
    {
        for (int j = 0; j < imageWidth; ++j)
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
        for (int j = imageWidth - 1; j >= 0; --j)
        {
            if (colors[pixels[j]] == color)
            {
                if (j > tempRight)
                {
                    tempRight = j + 20; // Pad value for borders
                }
                if (tempRight >= imageWidth)
                {
                    tempRight = imageWidth - 1;
                }
            }
        }
        pixels += pitch;
    }
    right = tempRight;

    int height = (bottom - top) + 1;
    int width = (right - left) + 1;

    subPicture->imageSizes()[0].setX(left);
    subPicture->imageSizes()[0].setY(top);
    subPicture->imageSizes()[0].setHeight(height);
    subPicture->imageSizes()[0].setWidth(width);

    subPicture->windowSizes()[0].setX(left);
    subPicture->windowSizes()[0].setY(top);
    subPicture->windowSizes()[0].setHeight(height);
    subPicture->windowSizes()[0].setWidth(width);

    QImage newImage = image.copy(left, top, width, height);
    _bitmap.setImg(newImage);
}

void SupBD::decode(SubPictureBD *subPicture)
{
    _palette = decodePalette(subPicture);
    _bitmap = decodeImage(subPicture, _palette.transparentIndex());
    _primaryColorIndex = _bitmap.primaryColorIndex(_palette, subtitleProcessor->getAlphaThreshold());

    // special case for subtitles that don't properly signal the image area
    if (subPicture->numberOfWindows() == 1 && _bitmap.image().width() == 1920 && _bitmap.image().height() == 1080)
    {
        findImageArea(subPicture);
    }
}

Palette SupBD::decodePalette(SubPictureBD *subPicture)
{
    bool fadeOut = false;
    int palIndex = 0;

    QVector<PaletteInfo> pl = subPicture->palettes[subPicture->paletteId()];

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
    subPicture->palettes[subPicture->paletteId()] = pl;
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

    QMap<int, Bitmap> bitmaps;

    for (int i = 0; i < objectIdxes.size(); ++i)
    {
        int imgObjIdx = objectIdxes[i];
        ImageObject imageObject = subPicture->imageObjectList[imgObjIdx];
        int w = imageObject.width();
        int h = imageObject.height();
        // always decode image obj 0, start with first entry in fragmentlist
        ImageObjectFragment fragment = imageObject.fragmentList()[0];
        qint64 startOfs = fragment.imageBufferOffset();

        if (w > subPicture->screenWidth() || h > subPicture->screenHeight())
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
        int line = 0;

        // just for multi-packet support, copy all of the image data in one common buffer
        QVector<uchar> buf = QVector<uchar>(imageObject.bufferSize());
        index = 0;
        for (int p = 0; p < imageObject.fragmentList().size(); ++p)
        {
            // copy data of all packet to one common buffer
            fragment = imageObject.fragmentList()[p];
            fileBuffer->getBytes(fragment.imageBufferOffset(), buf.data() + index, fragment.imagePacketSize());
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
        bitmaps[i] = bm;
    }

    if (bitmaps.size() == 2)
    {
        ImageObject imgObj1 = subPicture->imageObjectList[objectIdxes[0]];
        ImageObject imgObj2 = subPicture->imageObjectList[objectIdxes[1]];

        int imgObj1XOfs = imgObj1.x();
        int imgObj2XOfs = imgObj2.x();
        int imgObj1YOfs = imgObj1.y();
        int imgObj2YOfs = imgObj2.y();

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

        int right = (imgObj1XOfs + imgObj1Width) > (imgObj2XOfs + imgObj2Width) ?
                    (imgObj1XOfs + imgObj1Width) : (imgObj2XOfs + imgObj2Width);
        int resultWidth = (right - resultXOffset) + 1;

        QImage resultImage(1920, 1080, QImage::Format_ARGB32_Premultiplied);
        QPainter painter(&resultImage);
        painter.setCompositionMode(QPainter::CompositionMode_Source);
        painter.fillRect(resultImage.rect(), Qt::transparent);
        painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
        painter.drawImage(imgObj1XOfs, imgObj1YOfs, bitmaps[0].image(_palette));
        painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
        painter.drawImage(imgObj2XOfs, imgObj2YOfs, bitmaps[1].image(_palette));
        painter.end();

        resultImage = resultImage.convertToFormat(QImage::Format_Indexed8, _palette.colorTable());
        resultImage = resultImage.copy(resultXOffset, resultYOffset, resultWidth, resultHeight);

        Bitmap result = Bitmap(resultImage);

        return result;
    }

    return bitmaps[0];
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
