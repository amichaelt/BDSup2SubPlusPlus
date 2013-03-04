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

#include "suphd.h"
#include "subtitleprocessor.h"
#include "subpicturehd.h"
#include "Tools/filebuffer.h"
#include "Tools/bitstream.h"
#include "Tools/timeutil.h"
#include "bitmap.h"
#include "palette.h"
#include <QImage>

SupHD::SupHD(QString fileName, SubtitleProcessor *subtitleProcessor) :
    supFileName(fileName)
{
    this->subtitleProcessor = subtitleProcessor;
}

SupHD::~SupHD()
{
    if (!fileBuffer.isNull())
    {
        fileBuffer.reset();
    }
}

QImage SupHD::image()
{
    return _bitmap.image(_palette);
}

QImage SupHD::image(Bitmap &bitmap)
{
    return bitmap.image(_palette);
}

void SupHD::decode(int index)
{
    if (index < subPictures.size())
    {
        decode(subPictures[index]);
    }
    else
    {
        throw QString("Index: %1 out of bounds.\n").arg(QString::number(index));
    }
}

int SupHD::numFrames()
{
    return subPictures.size();
}

qint64 SupHD::endTime(int index)
{
    return subPictures[index].endTime();
}

qint64 SupHD::startTime(int index)
{
    return subPictures[index].startTime();
}

qint64 SupHD::startOffset(int index)
{
    return subPictures[index].imageBufferOffsetEven();
}

SubPicture *SupHD::subPicture(int index)
{
    return &subPictures[index];
}

void SupHD::readAllSupFrames()
{
    fileBuffer.reset(new FileBuffer(supFileName));

    int index = 0;
    int bufsize = (int)fileBuffer->getSize();
    SubPictureHD pic;
    subPictures.clear();

    try
    {
        while (index < bufsize)
        {
            if (subtitleProcessor->isCancelled())
            {
                throw QString("Cancelled by user!");
            }
            emit currentProgressChanged(index);

            if (fileBuffer->getWord(index) != 0x5350)
            {
                throw QString("ID 'SP' missing at index ").arg(QString::number(index, 16), 8, QChar('0'));
            }
            int masterIndex = index + 10; //end of header
            pic = SubPictureHD();
            // hard code size since it's not part of the format???
            pic.setScreenWidth(1920);
            pic.setScreenHeight(1080);

            subtitleProcessor->printX(QString("#%1\n").arg(QString::number(subPictures.size() + 1)));;

            pic.setStartTime(fileBuffer->getDWordLE(index += 2)); // read PTS
            int packetSize = fileBuffer->getDWord(index += 10);
            // offset to command buffer
            int ofsCmd = fileBuffer->getDWord(index += 4) + masterIndex;
            pic.setImageBufferSize(ofsCmd - (index + 4));
            index  = ofsCmd;
            int dcsq = fileBuffer->getWord(index);
            pic.setStartTime(pic.startTime() + (dcsq * 1024));

            subtitleProcessor->printX(QString("DCSQ start    ofs: %1  (%2)\n")
                                      .arg(QString::number(index, 16), 8, QChar('0'))
                                      .arg(TimeUtil::ptsToTimeStr(pic.startTime())));

            index+=2; // 2 bytes: dcsq
            int nextIndex = fileBuffer->getDWord(index) + masterIndex; // offset to next dcsq
            index += 5;  // 4 bytes: offset, 1 byte: start
            int cmd = 0;
            bool stopDisplay = false;
            bool stopCommand = false;
            int alphaSum = 0;
            int minAlphaSum = 256 * 256; // 256 fully transparent entries
            while (!stopDisplay)
            {
                cmd = fileBuffer->getByte(index++);
                switch (cmd)
                {
                case 0x01:
                {
                    subtitleProcessor->printX(QString("DCSQ start    ofs: %1  (%2)\n")
                                              .arg(QString::number(index, 16), 8, QChar('0'))
                                              .arg(TimeUtil::ptsToTimeStr(pic.startTime() + (dcsq * 1024))));
                    subtitleProcessor->printWarning("DCSQ start ignored due to missing DCSQ stop\n");
                } break;
                case 0x02:
                {
                    stopDisplay = true;
                    pic.setEndTime(pic.startTime() + (dcsq * 1024));

                    subtitleProcessor->printX(QString("DCSQ stop     ofs: %1  (%2)\n")
                                              .arg(QString::number(index, 16), 8, QChar('0'))
                                              .arg(TimeUtil::ptsToTimeStr(pic.endTime())));
                } break;
                case 0x83: // palette
                {
                    subtitleProcessor->print(QString("Palette info  ofs: %1\n").arg(QString::number(index, 16), 8, QChar('0')));

                    pic.setPaletteOffset(index);
                    index += 0x300;
                } break;
                case 0x84: // alpha
                {
                    subtitleProcessor->print(QString("Alpha info    ofs: %1\n").arg(QString::number(index, 16), 8, QChar('0')));                alphaSum = 0;

                    for (int i = index; i < index + 0x100; ++i)
                    {
                        alphaSum += fileBuffer->getByte(i);
                    }
                    if (alphaSum < minAlphaSum)
                    {
                        pic.setAlphaOffset(index);
                        minAlphaSum = alphaSum;
                    }
                    else
                    {
                        subtitleProcessor->printWarning(QString("Found faded alpha buffer -> alpha buffer skipped\n"));
                    }
                    index += 0x100;
                } break;
                case 0x85: // area
                {
                    pic.setX((fileBuffer->getByte(index) << 4) | (fileBuffer->getByte(index + 1) >> 4));
                    int imageWidth = (((fileBuffer->getByte(index + 1) &0xf) << 8) | (fileBuffer->getByte(index + 2)));
                    pic.setImageWidth((imageWidth - pic.x()) + 1);
                    pic.setY((fileBuffer->getByte(index + 3) <<4 ) | (fileBuffer->getByte(index + 4) >> 4));
                    int imageHeight = (((fileBuffer->getByte(index + 4) &0xf) << 8) | (fileBuffer->getByte(index + 5)));
                    pic.setImageHeight((imageHeight - pic.y()) + 1);

                    subtitleProcessor->print(QString("Area info     ofs: %1  (%2, %3) - (%4, %5)\n")
                                             .arg(QString::number(index, 16), 8, QChar('0'))
                                             .arg(QString::number(pic.x()))
                                             .arg(QString::number(pic.y()))
                                             .arg(QString::number(pic.x() + pic.imageWidth()))
                                             .arg(QString::number(pic.y() + pic.imageHeight())));

                    index += 6;
                } break;
                case 0x86: // even/odd offsets
                {
                    pic.setImageBufferOffsetEven(fileBuffer->getDWord(index) + masterIndex);
                    pic.setImageBufferOffsetOdd(fileBuffer->getDWord(index + 4) + masterIndex);

                    subtitleProcessor->print(QString("RLE buffers   ofs: %1  (even: %2, odd: %3)\n")
                                             .arg(QString::number(index, 16), 8, QChar('0'))
                                             .arg(QString::number(pic.imageBufferOffsetEven(), 16), 8, QChar('0'))
                                             .arg(QString::number(pic.imageBufferOffsetOdd(), 16), 8, QChar('0')));

                    index += 8;
                } break;
                case 0xff:
                {
                    if (stopCommand)
                    {
                        subtitleProcessor->printWarning(QString("DCSQ stop missing.\n"));

                        for (++index; index < bufsize; ++index)
                        {
                            if (fileBuffer->getByte(index++) != 0xff)
                            {
                                index--;
                                break;
                            }
                        }
                        stopDisplay = true;
                    } else {
                        index = nextIndex;
                        // add to display time
                        int d = fileBuffer->getWord(index);
                        dcsq = d;
                        nextIndex = fileBuffer->getDWord(index + 2) + masterIndex;
                        stopCommand = (index == nextIndex);

                        subtitleProcessor->print(QString("DCSQ          ofs: %1  (%2ms),    next DCSQ at ofs: %3\n")
                                                 .arg(QString::number(index, 16), 8, QChar('0'))
                                                 .arg(QString::number((d * 1024) / 90))
                                                 .arg(QString::number(nextIndex, 16), 8, QChar('0')));

                        index += 6;
                    }
                } break;
                default:
                {
                    throw QString("Unexpected command %1 at index %2").arg(cmd).arg(QString::number(index, 16), 8, QChar('0'));
                }
                }
            }
            index = masterIndex + packetSize;
            subPictures.push_back(pic);
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

    emit currentProgressChanged(bufsize);
}

void SupHD::decode(SubPictureHD &subPicture)
{
    _palette = decodePalette(subPicture);
    _bitmap = decodeImage(subPicture, _palette.transparentIndex());
    _primaryColorIndex = _bitmap.primaryColorIndex(_palette, subtitleProcessor->getAlphaThreshold());
}

void SupHD::decodeLine(QImage &trg, int trgOfs, int width, int maxPixels, BitStream* src)
{
    int x = 0;
    int pixelsLeft = 0;
    int sumPixels = 0;
    bool lf = false;

    while (src->bitsLeft() > 0 && sumPixels < maxPixels)
    {
        int rleType = src->readBits(1);
        int colorType = src->readBits(1);
        int color;
        int numPixels;

        if (colorType == 1)
        {
            color = src->readBits(8);
        }
        else
        {
            color = src->readBits(2); // Colors between 0 and 3 are stored in two bits
        }

        if (rleType == 1)
        {
            int rleSize = src->readBits(1);
            if (rleSize == 1)
            {
                numPixels = src->readBits(7) + 9;
                if (numPixels == 9)
                {
                    numPixels = width - x;
                }
            }
            else
            {
                numPixels = src->readBits(3) + 2;
            }
        }
        else
        {
            numPixels = 1;
        }

        if ((x + numPixels) == width)
        {
            src->syncToByte();
            lf = true;
        }
        sumPixels += numPixels;

        // write pixels to target
        if ((x + numPixels) > width)
        {
            pixelsLeft = (x + numPixels) - width;
            numPixels = width - x;
            lf = true;
        }
        else
        {
            pixelsLeft = 0;
        }

        uchar* pixels = trg.bits();
        for (int i = 0; i < numPixels; ++i)
        {
            pixels[trgOfs + x + i] = (uchar)color;
        }

        if (lf == true)
        {
            trgOfs += ((x + numPixels + (trg.bytesPerLine() - (x + numPixels)))
                    + (width + (trg.bytesPerLine() - width))); // skip odd/even line
            x = pixelsLeft;
            lf = false;
        }
        else
        {
            x += numPixels;
        }

        // copy remaining pixels to new line
        for (int i = 0; i < pixelsLeft; ++i)
        {
            pixels[trgOfs + i] = (uchar)color;
        }
    }
}

Palette SupHD::decodePalette(SubPictureHD &subPicture)
{
    int ofs = subPicture.paletteOffset();
    int alphaOfs = subPicture.alphaOffset();

    Palette palette(256);
    for (int i = 0; i < palette.size(); ++i)
    {
        // each palette entry consists of 3 bytes
        int y = fileBuffer->getByte(ofs++);
        int cr, cb;
        if (subtitleProcessor->getSwapCrCb())
        {
            cb = fileBuffer->getByte(ofs++);
            cr = fileBuffer->getByte(ofs++);
        }
        else
        {
            cr = fileBuffer->getByte(ofs++);
            cb = fileBuffer->getByte(ofs++);
        }
        // each alpha entry consists of 1 byte
        int alpha = 0xff - fileBuffer->getByte(alphaOfs++);
        if (alpha < subtitleProcessor->getAlphaCrop()) // to not mess with scaling algorithms, make transparent color black
        {
            palette.setRGB(i, qRgb(0, 0, 0));
        }
        else
        {
            palette.setYCbCr(i, y, cb, cr);
        }
        palette.setAlpha(i, alpha);
    }
    return palette;
}

Bitmap SupHD::decodeImage(SubPictureHD &subPicture, int transparentIndex)
{
    int w = subPicture.imageWidth();
    int h = subPicture.imageHeight();
    int warnings = 0;

    if (w > subPicture.screenWidth() || h > subPicture.screenHeight())
    {
        throw QString("Subpicture too large: %1x%2 at offset %3")
                .arg(QString::number(w))
                .arg(QString::number(h))
                .arg(QString::number(subPicture.imageBufferOffsetEven(), 16), 8, QChar('0'));
    }

    Bitmap bm(w, h, transparentIndex);

    int sizeEven = subPicture.imageBufferOffsetOdd() - subPicture.imageBufferOffsetEven();
    int sizeOdd = (subPicture.imageBufferSize() + subPicture.imageBufferOffsetEven()) - subPicture.imageBufferOffsetOdd();

    if (sizeEven <= 0 || sizeOdd <= 0)
    {
        throw QString("Corrupt buffer offset information");
    }

    QVector<uchar> evenBuf = QVector<uchar>(sizeEven);
    QVector<uchar> oddBuf  = QVector<uchar>(sizeOdd);

    for (int i = 0; i < evenBuf.size(); i++)
    {
        evenBuf.replace(i, (uchar)fileBuffer->getByte(subPicture.imageBufferOffsetEven() + i));
    }
    for (int i = 0; i < oddBuf.size(); ++i)
    {
        oddBuf.replace(i, (uchar)fileBuffer->getByte(subPicture.imageBufferOffsetOdd() + i));
    }
    // decode even lines
    BitStream even = BitStream(evenBuf);
    decodeLine(bm.image(), 0, w, w * ((h / 2) + (h & 1)), &even);
    // decode odd lines
    BitStream odd  = BitStream(oddBuf);
    decodeLine(bm.image(), w + (bm.image().bytesPerLine() - w), w, (h / 2) * w, &odd);

    if (warnings > 0)
    {
        subtitleProcessor->printWarning(QString("problems during RLE decoding of picture at offset %1\n")
                                        .arg(QString::number(subPicture.imageBufferOffsetEven(), 16), 8, QChar('0')));
    }

    return bm;
}
