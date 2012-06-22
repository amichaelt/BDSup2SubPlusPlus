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

#include <QImage>

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

QImage *SupBD::getImage()
{
    return bitmap->getImage(palette);
}

QImage *SupBD::getImage(Bitmap *bitmap)
{
    return bitmap->getImage(palette);
}

void SupBD::decode(int index)
{
    if (index < subPictures.size())
    {
        decode(subPictures.at(index));
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
    return subPictures.at(index)->isForced;
}

long SupBD::getEndTime(int index)
{
    return subPictures.at(index)->endTime;
}

long SupBD::getStartTime(int index)
{
    return subPictures.at(index)->startTime;
}

long SupBD::getStartOffset(int index)
{
    return subPictures.at(index)->getImgObj()->fragmentList.at(0)->imageBufferOfs;
}

SubPicture *SupBD::getSubPicture(int index)
{
    SubPictureBD* subPic = subPictures.at(index);
    return subPic;
}

void SupBD::readAllSupFrames()
{
    fileBuffer.reset(new FileBuffer(supFileName));

    int index = 0;
    int bufsize = (int)fileBuffer->getSize();
    SupBD::SupSegment* segment;
    SubPictureBD* pic = 0;
    SubPictureBD* picLast = 0;
    SubPictureBD* picTmp = 0;
    subPictures = QVector<SubPictureBD*>();
    int odsCtr = 0;
    int pdsCtr = 0;
    int odsCtrOld = 0;
    int pdsCtrOld = 0;
    int compNum = -1;
    int compNumOld = -1;
    int compCount = 0;
    long ptsPCS = 0;
    bool paletteUpdate = false;
    CompositionState cs = CompositionState::INVALID;

    try
    {
        while (index < bufsize)
        {
            if (subtitleProcessor->isCancelled())
            {
                if (subPictures.size() == 0)
                {
                    throw QString("Cancelled by user!");
                }
            }
            emit currentProgressChanged(index);
            segment = readSegment(index);
            QString out;
            QString so; // hack to return string
            switch (segment->type)
            {
            case 0x14:
            {
                out = QString("PDS ofs:%1, size:%2").arg(QString::number(index, 16), 8, QChar('0'))
                                                    .arg(QString::number(segment->size, 16), 4, QChar('0'));
                if (compNum != compNumOld)
                {
                    if (pic != 0)
                    {
                        so = QString("");
                        int ps = parsePDS(segment, pic, so);
                        if (ps >= 0)
                        {
                            subtitleProcessor->print(QString("%1, %2\n").arg(out).arg(so));

                            if (ps > 0) // don't count empty palettes
                            {
                                pdsCtr++;
                            }
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
                }
                else
                {
                    subtitleProcessor->print(QString(out + ", comp # unchanged -> ignored\n"));
                }
            } break;
            case 0x15:
            {
                out = QString("ODS ofs:%1, size%2").arg(QString::number(index, 16), 8, QChar('0'))
                                                   .arg(QString::number(segment->size, 16), 4, QChar('0'));
                if (compNum != compNumOld)
                {
                    if (!paletteUpdate)
                    {
                        if (pic != 0)
                        {
                            so = QString("");
                            if (parseODS(segment, pic, so))
                            {
                                odsCtr++;
                            }
                            if (!so.isNull())
                            {
                                out += ", "+so;
                            }

                            subtitleProcessor->print(QString("%1, img size: %2*%3\n")
                                                     .arg(out)
                                                     .arg(QString::number(pic->getImageWidth()))
                                                     .arg(QString::number(pic->getImageHeight())));
                        }
                        else
                        {
                            subtitleProcessor->print(QString(out + "\n"));
                            subtitleProcessor->printWarning(QString("missing PTS start -> ignored\n"));
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
                    subtitleProcessor->print(QString(out + ", comp # unchanged -> ignored\n"));
                }
            } break;
            case 0x16:
            {
                compNum = fileBuffer->getWord(segment->offset + 5);
                cs = getCompositionState(segment);
                paletteUpdate = fileBuffer->getByte(segment->offset + 8) == 0x80;
                ptsPCS = segment->timestamp;
                if (segment->size >= 0x13)
                {
                    compCount = 1; // could be also 2, but we'll ignore this for the moment
                }
                else
                {
                    compCount = 0;
                }
                if (cs == CompositionState::INVALID)
                {
                    subtitleProcessor->printWarning(QString("Illegal composition state at offset %1\n")
                                                    .arg(QString::number(index, 16), 8, QChar('0')));
                }
                else if (cs == CompositionState::EPOCH_START)
                {
                    // new frame
                    if (subPictures.size() > 0 && (odsCtr==0 || pdsCtr==0))
                    {
                        subtitleProcessor->printWarning(QString("missing PDS/ODS: last epoch is discarded\n"));

                        subPictures.remove(subPictures.size()-1);
                        compNumOld = compNum - 1;
                        if (subPictures.size() > 0)
                        {
                            picLast = subPictures.at(subPictures.size()-1);
                        }
                        else
                        {
                            picLast = 0;
                        }
                    }
                    else
                    {
                        picLast = pic;
                    }
                    pic = new SubPictureBD();
                    subPictures.push_back(pic); // add to list
                    pic->startTime = segment->timestamp;

                    subtitleProcessor->printX(QString("#> %1 (%2)\n")
                                              .arg(QString::number(subPictures.size()))
                                              .arg(TimeUtil::ptsToTimeStr(pic->startTime)));

                    so = QString("");
                    parsePCS(segment, pic, so);
                    // fix end time stamp of previous pic if still missing
                    if (picLast != 0 && picLast->endTime == 0)
                    {
                        picLast->endTime = pic->startTime;
                    }

                    out = QString("PCS ofs:%1, START, size:%2, comp#: %3, forced: %4").arg(QString::number(index, 16), 8, QChar('0'))
                                                                                      .arg(QString::number(segment->size, 16), 4, QChar('0'))
                                                                                      .arg(QString::number(compNum))
                                                                                      .arg(pic->isForced ? "true" : "false");
                    if (!so.isNull())
                    {
                        out+= ", " + so + "\n";
                    }
                    else
                    {
                        out+="\n";
                    }
                    out += QString("PTS start: %1").arg(TimeUtil::ptsToTimeStr(pic->startTime));
                    out += QString(", screen size: %1*%2\n").arg(QString::number(pic->width)).arg(QString::number(pic->height));
                    odsCtr = 0;
                    pdsCtr = 0;
                    odsCtrOld = 0;
                    pdsCtrOld = 0;
                    picTmp = 0;

                    subtitleProcessor->print(out);
                }
                else
                {
                    if (pic == 0)
                    {
                        subtitleProcessor->printWarning(QString("missing start of epoch at offset %1\n")
                                                        .arg(QString::number(index, 16), 8, QChar('0')));
                        break;
                    }
                    out = QString("PCS ofs:%1, ").arg(QString::number(index, 16), 8, QChar('0'));
                    switch ((int)cs)
                    {
                    case (int)CompositionState::EPOCH_CONTINUE:
                    {
                        out += QString("CONT, ");
                    } break;
                    case (int)CompositionState::ACQU_POINT:
                    {
                        out += QString("ACQU, ");
                    } break;
                    case (int)CompositionState::NORMAL:
                    {
                        out += QString("NORM, ");
                    } break;
                    }
                    out += QString(" size: %1, comp#: %2, forced: %3").arg(QString::number(segment->size, 16), 4, QChar('0'))
                                                                      .arg(QString::number(compNum))
                                                                      .arg(pic->isForced ? "true" : "false");
                    if (compNum != compNumOld)
                    {
                        so = QString("");
                        // store state to be able to revert to it
                        picTmp = new SubPictureBD(pic);
                        picTmp->endTime = ptsPCS;
                        // create new pic
                        parsePCS(segment, pic, so);
                    }
                    if (!so.isNull())
                    {
                        out+=", "+so;
                    }
                    out += QString(", pal update: %1\n").arg(paletteUpdate ? "true" : "false");
                    out += QString("PTS: %1\n").arg(TimeUtil::ptsToTimeStr(segment->timestamp));

                    subtitleProcessor->print(out);
                }
            } break;
            case 0x17:
            {
                out = QString("WDS ofs:%1, size:%2").arg(QString::number(index, 16), 8, QChar('0'))
                                                    .arg(QString::number(segment->size, 16), 4, QChar('0'));
                if (pic != 0)
                {
                    parseWDS(segment, pic);

                    subtitleProcessor->print(QString("%1, dim: %2*%3\n")
                                             .arg(out)
                                             .arg(QString::number(pic->winWidth))
                                             .arg(QString::number(pic->winHeight)));
                }
                else
                {
                    subtitleProcessor->print(QString(out + "\n"));
                    subtitleProcessor->printWarning(QString("Missing PTS start -> ignored\n"));
                }
            } break;
            case 0x80:
            {
                subtitleProcessor->print(QString("END ofs: %1\n").arg(QString::number(index, 16), 8, QChar('0')));

                // decide whether to store this last composition section as caption or merge it
                if (cs == CompositionState::EPOCH_START)
                {
                    if (compCount>0 && odsCtr>odsCtrOld
                                    && compNum != compNumOld
                                    && picMergable(picLast, pic))
                    {
                        // the last start epoch did not contain any (new) content
                        // and should be merged to the previous frame
                        subPictures.remove(subPictures.size() - 1);
                        pic = picLast;
                        if (subPictures.size() > 0)
                        {
                            picLast = subPictures.at(subPictures.size()-1);
                        }
                        else
                        {
                            picLast = 0;
                        }

                        subtitleProcessor->printX(QString("#< caption merged\n"));
                    }
                }
                else
                {
                    long startTime = 0;
                    if (pic != 0)
                    {
                        startTime = pic->startTime;  // store
                        pic->startTime = ptsPCS;    // set for testing merge
                    }

                    if (compCount>0 && odsCtr>odsCtrOld
                                    && compNum != compNumOld
                                    && !picMergable(picTmp, pic))
                    {
                        // last PCS should be stored as separate caption
                        if (odsCtr - odsCtrOld > 1 || pdsCtr - pdsCtrOld > 1)
                        {
                            subtitleProcessor->printWarning(QString("multiple PDS/ODS definitions: result may be erratic\n"));
                        }
                        // replace pic with picTmp (deepCopy created before new PCS)
                        subPictures.replace(subPictures.size() - 1, picTmp); // replace in list
                        picLast = picTmp;
                        subPictures.push_back(pic); // add to list

                        subtitleProcessor->printX(QString("#< %1 (%2)\n")
                                                  .arg(QString::number(subPictures.size()))
                                                  .arg(TimeUtil::ptsToTimeStr(pic->startTime)));

                        odsCtrOld = odsCtr;
                    }
                    else
                    {
                        if (pic != 0)
                        {
                            // merge with previous pic
                            pic->startTime = startTime; // restore
                            pic->endTime = ptsPCS;
                            // for the unlikely case that forced flag changed during one captions
                            if (picTmp != 0 && picTmp->isForced)
                            {
                                pic->isForced = true;
                            }
                            if (pdsCtr > pdsCtrOld || paletteUpdate)
                            {
                                subtitleProcessor->printWarning(QString("palette animation: result may be erratic\n"));
                            }
                        }
                        else
                        {
                            subtitleProcessor->printWarning(QString("end without at least one epoch start\n"));
                        }
                    }
                }
                pdsCtrOld = pdsCtr;
                compNumOld = compNum;
            } break;
            default:
            {
                subtitleProcessor->printWarning(QString("<unknown> %1 ofs: %2\n")
                                                .arg(QString::number(segment->type, 16), 2, QChar('0'))
                                                .arg(QString::number(index, 16), 8, QChar('0')));
            } break;
            }
            index += 13; // header size
            index += segment->size;
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
    }

    // check if last frame is valid
    if (subPictures.size() > 0 && (odsCtr == 0 || pdsCtr == 0))
    {
        subtitleProcessor->printWarning(QString("missing PDS/ODS: last epoch is discarded\n"));

        subPictures.remove(subPictures.size()-1);
    }

    emit currentProgressChanged(bufsize);
    // count forced frames
    numForcedFrames = 0;
    for (auto subPicture : subPictures)
    {
        if (subPicture->isForced)
        {
            numForcedFrames++;
        }
    }

    subtitleProcessor->printX(QString("\nDetected %1 forced captions.\n")
                              .arg(QString::number(numForcedFrames)));
}

QVector<uchar> SupBD::encodeImage(Bitmap *bm)
{
    QVector<uchar> bytes;
    uchar color;
    int ofs;
    int len;

    for (int y = 0; y < bm->getHeight(); ++y)
    {
        uchar* pixels = bm->getImg()->scanLine(y);
        ofs = 0;
        int x;
        for (x = 0; x < bm->getWidth(); x += len, ofs += len)
        {
            color = pixels[ofs];
            for (len = 1; (x + len) < bm->getWidth(); ++len)
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
        if (x == bm->getWidth())
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

QVector<uchar> SupBD::createSupFrame(SubPicture *subPicture, Bitmap *bm, Palette *pal)
{
    // the last palette entry must be transparent
    if (pal->getSize() > 255 && pal->getAlpha(255) > 0)
    {
        // quantize image
        QuantizeFilter qf;
        Bitmap* bmQ = new Bitmap(bm->getWidth(), bm->getHeight());
        QVector<QRgb> ct = qf.quantize(bm->toARGB(pal), bmQ->getImg(), bm->getWidth(), bm->getHeight(), 255, false, false);
        int size = ct.size();

        subtitleProcessor->print(QString("Palette had to be reduced from %1 to %2 entries.\n")
                                 .arg(QString::number(pal->getSize()))
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
            pal->setARGB(i, ct[i]);
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
    int palSize = bm->getHighestColorIndex(pal) + 1;
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
        int yMax = (subPicture->height - subPicture->getImageHeight()) - (2 * subtitleProcessor->getCropOfsY());
        if (yOfs > yMax)
        {
            yOfs = yMax;
        }
    }

    int h = subPicture->height - (2 * subtitleProcessor->getCropOfsY());

    QVector<uchar> buf(size);
    int index = 0;

    int fpsId = getFpsId(subtitleProcessor->getFPSTrg());

    /* time (in 90kHz resolution) needed to initialize (clear) the screen buffer
       based on the composition pixel rate of 256e6 bit/s - always rounded up */
    int frameInitTime = (((subPicture->width * subPicture->height) * 9) + 3199) / 3200; // better use default height here
    /* time (in 90kHz resolution) needed to initialize (clear) the window area
       based on the composition pixel rate of 256e6 bit/s - always rounded up
       Note: no cropping etc. -> window size == image size */
    int windowInitTime = (((bm->getWidth() * bm->getHeight()) * 9) + 3199) / 3200;
    /* time (in 90kHz resolution) needed to decode the image
       based on the decoding pixel rate of 128e6 bit/s - always rounded up  */
    int imageDecodeTime = (((bm->getWidth() * bm->getHeight()) * 9) + 1599) / 1600;
    // write PCS start
    packetHeader.replace(10, 0x16);                                             // ID
    int dts = (int)subPicture->startTime - (frameInitTime + windowInitTime);
    NumberUtil::setDWord(packetHeader, 2, (int)subPicture->startTime);				// PTS
    NumberUtil::setDWord(packetHeader, 6, dts);								// DTS
    NumberUtil::setWord(packetHeader, 11, headerPCSStart.size());			// size
    for (int i = 0; i < packetHeader.size(); ++i)
    {
        buf.replace(index++, packetHeader[i]);
    }
    NumberUtil::setWord(headerPCSStart, 0, subPicture->width);
    NumberUtil::setWord(headerPCSStart, 2, h);								// cropped height
    NumberUtil::setByte(headerPCSStart, 4, fpsId);
    NumberUtil::setWord(headerPCSStart, 5, subPicture->compNum);
    headerPCSStart.replace(14, (subPicture->isForced ? (uchar)0x40 : 0));
    NumberUtil::setWord(headerPCSStart, 15, subPicture->getOfsX());
    NumberUtil::setWord(headerPCSStart, 17, yOfs);
    for (int i = 0; i < headerPCSStart.size(); ++i)
    {
        buf.replace(index++, headerPCSStart[i]);
    }

    // write WDS
    packetHeader.replace(10, 0x17);											// ID
    int timeStamp = (int)subPicture->startTime - windowInitTime;
    NumberUtil::setDWord(packetHeader, 2, timeStamp);						// PTS (keep DTS)
    NumberUtil::setWord(packetHeader, 11, headerWDS.size());				// size
    for (int i = 0; i < packetHeader.size(); ++i)
    {
        buf.replace(index++, packetHeader[i]);
    }
    NumberUtil::setWord(headerWDS, 2, subPicture->getOfsX());
    NumberUtil::setWord(headerWDS, 4, yOfs);
    NumberUtil::setWord(headerWDS, 6, bm->getWidth());
    NumberUtil::setWord(headerWDS, 8, bm->getHeight());
    for (int i = 0; i < headerWDS.size(); ++i)
    {
        buf.replace(index++, headerWDS[i]);
    }

    // write PDS
    packetHeader.replace(10, 0x14);											// ID
    NumberUtil::setDWord(packetHeader, 2, dts);								// PTS (=DTS of PCS/WDS)
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
        buf.replace(index++, pal->getY()[i]);									// Y
        buf.replace(index++, pal->getCr()[i]);									// Cr
        buf.replace(index++, pal->getCb()[i]);									// Cb
        buf.replace(index++, qAlpha(pal->getColorTable()[i]));   				// Alpha
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
    NumberUtil::setDWord(packetHeader, 6, dts);								// DTS
    NumberUtil::setWord(packetHeader, 11, headerODSFirst.size() + bufSize);	// size
    for (int i = 0; i < packetHeader.size(); ++i)
    {
        buf.replace(index++, packetHeader[i]);
    }
    int marker = ((numAddPackets == 0) ? 0xC0000000 : 0x80000000);
    NumberUtil::setDWord(headerODSFirst, 3, marker | (rleBuf.size() + 4));
    NumberUtil::setWord(headerODSFirst, 7, bm->getWidth());
    NumberUtil::setWord(headerODSFirst, 9, bm->getHeight());
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
    NumberUtil::setDWord(packetHeader, 6, 0);								// DTS (0) (keep PTS of ODS)
    NumberUtil::setWord(packetHeader, 11, 0);								// size
    for (int i = 0; i < packetHeader.size(); ++i)
    {
        buf.replace(index++, packetHeader[i]);
    }

    // write PCS end
    packetHeader.replace(10, 0x16);											// ID
    NumberUtil::setDWord(packetHeader, 2, (int)subPicture->endTime);				// PTS
    dts = (int)subPicture->startTime - 1;
    NumberUtil::setDWord(packetHeader, 6, dts);								// DTS
    NumberUtil::setWord(packetHeader, 11, headerPCSEnd.size());				// size
    for (int i = 0; i < packetHeader.size(); ++i)
    {
        buf.replace(index++, packetHeader[i]);
    }
    NumberUtil::setWord(headerPCSEnd,0, subPicture->width);
    NumberUtil::setWord(headerPCSEnd,2, h);									// cropped height
    NumberUtil::setByte(headerPCSEnd,4, fpsId);
    NumberUtil::setWord(headerPCSEnd,5, subPicture->compNum+1);
    for (int i = 0; i < headerPCSEnd.size(); ++i)
    {
        buf.replace(index++, headerPCSEnd[i]);
    }

    // write WDS
    packetHeader.replace(10, 0x17);											// ID
    timeStamp = (int)subPicture->endTime - windowInitTime;
    NumberUtil::setDWord(packetHeader, 2, timeStamp);						// PTS (keep DTS of PCS)
    NumberUtil::setWord(packetHeader, 11, headerWDS.size());				// size
    for (int i = 0; i < packetHeader.size(); ++i)
    {
        buf.replace(index++, packetHeader[i]);
    }
    NumberUtil::setWord(headerWDS, 2, subPicture->getOfsX());
    NumberUtil::setWord(headerWDS, 4, yOfs);
    NumberUtil::setWord(headerWDS, 6, bm->getWidth());
    NumberUtil::setWord(headerWDS, 8, bm->getHeight());
    for (int i = 0; i < headerWDS.size(); ++i)
    {
        buf.replace(index++, headerWDS[i]);
    }

    // write END
    packetHeader.replace(10, 0x80);										// ID
    NumberUtil::setDWord(packetHeader, 2, dts);								// PTS (DTS of end PCS)
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
    return getFpsFromID(subPictures.at(index)->type);
}

SupBD::SupSegment *SupBD::readSegment(int offset)
{
    SupSegment* segment = new SupSegment;
    if (fileBuffer->getWord(offset) != 0x5047)
    {
        throw QString("PG missing at index ").arg(QString::number(offset, 16), 8, QChar('0'));
    }
    segment->timestamp = fileBuffer->getDWord(offset += 2); // read PTS
    offset += 4; /* ignore DTS */
    segment->type = fileBuffer->getByte(offset += 4);
    segment->size = fileBuffer->getWord(++offset);
    segment->offset = offset + 2;
    return segment;
}

bool SupBD::picMergable(SubPictureBD* a, SubPictureBD* b)
{
    bool eq = false;
    if (a != 0 && b != 0)
    {
        if (a->endTime == 0 || (b->startTime - a->endTime) < subtitleProcessor->getMergePTSdiff())
        {
            ImageObject* ao = a->getImgObj();
            ImageObject* bo = b->getImgObj();
            if (ao != 0 && bo != 0)
            {
                if ((ao->bufferSize == bo->bufferSize) && (ao->width == bo->width) && (ao->height == bo->height))
                {
                    eq = true;
                }
            }
        }
    }
    return eq;
}

void SupBD::parsePCS(SupSegment* segment, SubPictureBD* subPicture, QString msg)
{
    int index = segment->offset;
    if (segment->size >= 4)
    {
        subPicture->width  = fileBuffer->getWord(index);            // video_width
        subPicture->height = fileBuffer->getWord(index + 2);        // video_height
        int type = fileBuffer->getByte(index + 4);                  // hi nibble: frame_rate, lo nibble: reserved
        int num  = fileBuffer->getWord(index + 5);                  // composition_number
        // skipped:
        // 8bit  composition_state: 0x00: normal, 		0x40: acquisition point
        //							0x80: epoch start,	0xC0: epoch continue, 6bit reserved
        // 8bit  palette_update_flag (0x80), 7bit reserved
        int palID = fileBuffer->getByte(index + 9);                 // 8bit  palette_id_ref
        int coNum = fileBuffer->getByte(index + 10);                // 8bit  number_of_composition_objects (0..2)
        if (coNum > 0)
        {
            // composition_object:
            int objID = fileBuffer->getWord(index + 11);            // 16bit object_id_ref
            msg = QString("palID: %1, objID: %2").arg(QString::number(palID)).arg(QString::number(objID));
            ImageObject* imgObj;
            if (objID >= subPicture->imageObjectList.size())
            {
                imgObj = new ImageObject;
                subPicture->imageObjectList.push_back(imgObj);
            }
            else
            {
                imgObj = subPicture->getImgObj(objID);
            }
            imgObj->paletteID = palID;
            subPicture->objectID = objID;

            // skipped:  8bit  window_id_ref
            if (segment->size >= 0x13)
            {
                subPicture->type = type;
                // object_cropped_flag: 0x80, forced_on_flag = 0x040, 6bit reserved
                int forcedCropped = fileBuffer->getByte(index + 14);
                subPicture->compNum = num;
                subPicture->isForced = ((forcedCropped & 0x40) == 0x40);
                imgObj->xOfs  = fileBuffer->getWord(index + 15);     // composition_object_horizontal_position
                imgObj->yOfs = fileBuffer->getWord(index + 17);		// composition_object_vertical_position
            }
        }
    }
}

int SupBD::parsePDS(SupSegment *segment, SubPictureBD *subPicture, QString msg)
{
    int index = segment->offset;
    int paletteID = fileBuffer->getByte(index);	// 8bit palette ID (0..7)
    // 8bit palette version number (incremented for each palette change)
    int paletteUpdate = fileBuffer->getByte(index + 1);
    if (subPicture->palettes.isEmpty())
    {
        subPicture->palettes = QVector<QVector<PaletteInfo*>*>();
        for (int i = 0; i < 8; ++i)
        {
            subPicture->palettes.push_back(new QVector<PaletteInfo*>);
        }
    }
    if (paletteID > 7)
    {
        msg = QString("Illegal palette id at offset %1").arg(QString::number(index, 16), 8, QChar('0'));
        return -1;
    }
    //
    QVector<PaletteInfo*>* al = subPicture->palettes.at(paletteID);
    if (al == 0)
    {
        al = new QVector<PaletteInfo*>;
    }
    PaletteInfo* p = new PaletteInfo;
    p->paletteSize = (segment->size - 2) / 5;
    p->paletteOfs = index + 2;
    al->push_back(p);
    msg = QString("ID: %1, update: %2, %3 entries").arg(QString::number(paletteID))
                                                   .arg(QString::number(paletteUpdate))
                                                   .arg(QString::number(p->paletteSize));
    return p->paletteSize;
}

bool SupBD::parseODS(SupSegment *segment, SubPictureBD* subPicture, QString msg)
{
    int index = segment->offset;
    ImageObjectFragment* info;
    int objID = fileBuffer->getWord(index);         // 16bit object_id
    int objVer = fileBuffer->getByte(index+1);		// 16bit object_id
    int objSeq = fileBuffer->getByte(index+3);		// 8bit  first_in_sequence (0x80),
                                                    // last_in_sequence (0x40), 6bits reserved
    bool first = ((objSeq & 0x80) == 0x80);
    bool last  = ((objSeq & 0x40) == 0x40);

    ImageObject* imgObj;
    if (objID >= subPicture->imageObjectList.size())
    {
        imgObj = new ImageObject;
        subPicture->imageObjectList.push_back(imgObj);
    }
    else
    {
        imgObj = subPicture->getImgObj(objID);
    }

    if (imgObj->fragmentList.isEmpty() || first)       // 8bit  object_version_number
    {
        // skipped:
        //  24bit object_data_length - full RLE buffer length (including 4 bytes size info)
        int width  = fileBuffer->getWord(index + 7);  // object_width
        int height = fileBuffer->getWord(index + 9);  // object_height

        if (width <= subPicture->width && height <= subPicture->height)
        {
            imgObj->fragmentList = QVector<ImageObjectFragment*>();
            info = new ImageObjectFragment;
            info->imageBufferOfs = index + 11;
            info->imagePacketSize = segment->size - ((index + 11) - segment->offset);
            imgObj->fragmentList.push_back(info);
            imgObj->bufferSize = info->imagePacketSize;
            imgObj->height = height;
            imgObj->width  = width;
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
        info = new ImageObjectFragment;
        info->imageBufferOfs = index + 4;
        info->imagePacketSize = segment->size - (index + 4 - segment->offset);
        imgObj->fragmentList.push_back(info);
        imgObj->bufferSize += info->imagePacketSize;
        msg = QString("ID: %1, update: %2, seq: %3%4%5").arg(QString::number(objID))
                                                        .arg(QString::number(objVer))
                                                        .arg(first ? QString("first") : QString(""))
                                                        .arg((first && last) ? QString("/") : QString(""))
                                                        .arg(last ? QString("last") : QString(""));
        return false;
    }
}

void SupBD::parseWDS(SupSegment* segment, SubPictureBD* subPicture)
{
    int index = segment->offset;

    if (segment->size >= 10)
    {
        // skipped:
        // 8bit: number of windows (currently assumed 1, 0..2 is legal)
        // 8bit: window id (0..1)
        subPicture->xWinOfs   = fileBuffer->getWord(index+2);	// window_horizontal_position
        subPicture->yWinOfs   = fileBuffer->getWord(index+4);	// window_vertical_position
        subPicture->winWidth  = fileBuffer->getWord(index+6);	// window_width
        subPicture->winHeight = fileBuffer->getWord(index+8);	// window_height
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

void SupBD::decode(SubPictureBD *subPicture)
{
    palette = decodePalette(subPicture);
    bitmap = decodeImage(subPicture, palette->getTransparentIndex());
    primaryColorIndex = bitmap->getPrimaryColorIndex(palette, subtitleProcessor->getAlphaThreshold());
}

Palette *SupBD::decodePalette(SubPictureBD *subPicture)
{
    bool fadeOut = false;
    int palIndex = 0;
    QVector<PaletteInfo*>* pl = subPicture->palettes.at(subPicture->getImgObj()->paletteID);
    if (pl == 0)
    {
        throw QString("Palette ID out of bounds.");
    }

    Palette* palette = new Palette(256, subtitleProcessor->usesBT601());
    // by definition, index 0xff is always completely transparent
    // also all entries must be fully transparent after initialization

    for (int j = 0; j < pl->size(); j++)
    {
        PaletteInfo* p = pl->at(j);
        int index = p->paletteOfs;
        for (int i = 0; i < p->paletteSize; ++i)
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

            int alphaOld = palette->getAlpha(palIndex);
            // avoid fading out
            if (alpha >= alphaOld)
            {
                if (alpha < subtitleProcessor->getAlphaCrop()) // to not mess with scaling algorithms, make transparent color black
                {
                    y = 16;
                    cr = 128;
                    cb = 128;
                }
                palette->setAlpha(palIndex, alpha);
            }
            else
            {
                fadeOut = true;
            }

            palette->setYCbCr(palIndex, y, cb, cr);
            index++;
        }
    }
    if (fadeOut)
    {
        subtitleProcessor->printWarning(QString("fade out detected -> patched palette\n"));
    }
    return palette;
}

Bitmap* SupBD::decodeImage(SubPictureBD *subPicture, int transparentIndex)
{
    int w = subPicture->getImageWidth();
    int h = subPicture->getImageHeight();
    // always decode image obj 0, start with first entry in fragmentlist
    ImageObjectFragment* fragment = subPicture->getImgObj()->fragmentList.at(0);
    long startOfs = fragment->imageBufferOfs;

    if (w > subPicture->width || h > subPicture->height)
    {
        throw QString("Subpicture too large: %1x%2 at offset %3")
                .arg(QString::number(w))
                .arg(QString::number(h))
                .arg(QString::number(startOfs, 16), 8, QChar('0'));
    }

    Bitmap* bm = new Bitmap(w, h, transparentIndex);

    int b;
    int index = 0;
    int ofs = 0;
    int size = 0;
    int xpos = 0;

    // just for multi-packet support, copy all of the image data in one common buffer
    QVector<uchar> buf = QVector<uchar>(subPicture->getImgObj()->bufferSize);
    index = 0;
    for (int p = 0; p < subPicture->getImgObj()->fragmentList.size(); ++p)
    {
        // copy data of all packet to one common buffer
        fragment = subPicture->getImgObj()->fragmentList.at(p);
        for (int i = 0; i < fragment->imagePacketSize; ++i)
        {
            buf[index + i] = (uchar)fileBuffer->getByte(fragment->imageBufferOfs + i);
        }
        index += fragment->imagePacketSize;
    }

    index = 0;

    do
    {
        b = buf[index++] & 0xff;
        if (b == 0)
        {
            b = buf[index++] & 0xff;
            if (b == 0)
            {
                // next line
                ofs = (ofs / (w + (bm->getImg()->bytesPerLine() - w))) * (w + (bm->getImg()->bytesPerLine() - w));
                if (xpos < (w + (bm->getImg()->bytesPerLine() - w)))
                {
                    ofs += (w + (bm->getImg()->bytesPerLine() - w));
                }
                xpos = 0;
            }
            else
            {
                if ((b & 0xC0) == 0x40)
                {
                    // 00 4x xx -> xxx zeroes
                    size = ((b - 0x40) << 8) + (buf[index++] & 0xff);

                    uchar* pixels = bm->getImg()->bits();
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

                    uchar* pixels = bm->getImg()->bits();
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

                    uchar* pixels = bm->getImg()->bits();
                    for (int i = 0; i < size; ++i)
                    {
                        pixels[ofs++] = (uchar)b;
                    }
                    xpos += size;
                }
                else
                {
                    uchar* pixels = bm->getImg()->bits();
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
            uchar* pixels = bm->getImg()->bits();
            pixels[ofs++] = (uchar)b;
            xpos++;
        }
    } while (index < buf.size());

    return bm;
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
