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

#include "substreamdvd.h"
#include "palette.h"
#include "subpicturedvd.h"
#include "Tools/filebuffer.h"
#include "types.h"
#include "subtitleprocessor.h"
#include "imageobjectfragment.h"

#include <QImage>

SubstreamDVD::SubstreamDVD() :
    bitmap(0, 0),
    srcPalette(defaultPalR, defaultPalG, defaultPalB, defaultAlpha, true),
    palette(4, true)
{
}

SubstreamDVD::~SubstreamDVD()
{
    if (!fileBuffer.isNull())
    {
        fileBuffer.reset();
    }
}

Palette SubstreamDVD::decodePalette(SubPictureDVD &pic, Palette &palette, int alphaCrop)
{
    Palette miniPalette(4, true);

    for (int i = 0; i < miniPalette.size(); ++i)
    {
        int a = (pic.alpha[i] * 0xff) / 0xf;
        if (a >= alphaCrop)
        {
            miniPalette.setRGB(i, palette.colorTable()[pic.pal[i]]);
            miniPalette.setAlpha(i, a);
        }
        else
        {
            miniPalette.setARGB(i, 0);
        }
    }
    return miniPalette;
}

QVector<uchar> SubstreamDVD::encodeLines(Bitmap &bitmap, bool even)
{
    int ofs = 0;
    uchar color;
    int len;
    int y;
    QVector<uchar> nibbles;

    if (even)
    {
        y = 0;
    }
    else
    {
        y = 1;
    }

    for (; y < bitmap.height(); y += 2)
    {
        uchar* pixels = bitmap.image().scanLine(y);
        ofs = 0;
        for (int x = 0; x < bitmap.width(); x += len, ofs += len)
        {
            color = pixels[ofs];
            for (len = 1; x + len < bitmap.width(); ++len)
            {
                if (pixels[ofs + len] != color)
                {
                    break;
                }
            }
            if (len < 4)
            {
                nibbles.push_back((uchar)((len << 2) | (color & 3)));
            }
            else if (len < 0x10)
            {
                nibbles.push_back((uchar)(len >> 2));
                nibbles.push_back((uchar)((len << 2) | (color & 3)));
            }
            else if (len < 0x40)
            {
                nibbles.push_back((uchar)0);
                nibbles.push_back((uchar)(len >> 2));
                nibbles.push_back((uchar)((len << 2) | (color & 3)));
            }
            else if ((x + len) == bitmap.width())
            {
                nibbles.push_back((uchar)(0));
                nibbles.push_back((uchar)(0));
                nibbles.push_back((uchar)(0));
                nibbles.push_back(color);
            }
            else
            {
                if (len > 0xff)
                {
                    len = 0xff;
                }
                nibbles.push_back((uchar)(0));
                nibbles.push_back((uchar)(len >> 6));
                nibbles.push_back((uchar)(len >> 2));
                nibbles.push_back((uchar)((len << 2) | (color & 3)));
            }
        }
        if ((nibbles.size() & 1) == 1)
        {
            nibbles.push_back((uchar)(0));
        }
    }
    // end buffer with line feed
    nibbles.push_back((uchar)(0));
    nibbles.push_back((uchar)(0));
    nibbles.push_back((uchar)(0));
    nibbles.push_back((uchar)(0));

    int size =  nibbles.size() / 2; // number of bytes
    QVector<uchar> retval(size);
    QVectorIterator<uchar> it(nibbles);
    for (int i = 0; i < size; ++i)
    {
        int hi = (it.next() & 0xf);
        int lo = (it.next() & 0xf);
        retval[i] = (uchar)((hi << 4) | lo);
    }
    return retval;
}

Bitmap SubstreamDVD::decodeImage(SubPictureDVD &pic, int transIdx)
{
    int width = pic.originalWidth();
    int height = pic.originalHeight();
    int warnings = 0;

    ImageObjectFragment* fragment = pic.rleFragments.at(0);
    long startOfs = fragment->imageBufferOffset();

    if (width > pic.width() || height > pic.height())
    {
        subtitleProcessor->printWarning(QString("Subpicture too large: %1x%2 at offset %3\n")
                       .arg(QString::number(width))
                       .arg(QString::number(height))
                       .arg(QString::number(startOfs, 16), 8, QChar('0')));
    }

    Bitmap bm(width, height, transIdx);

    QVector<uchar> buf(pic.rleSize());
    int index = 0;

    int sizeEven;
    int sizeOdd;

    if (pic.oddOffset() > pic.evenOffset())
    {
        sizeEven = pic.oddOffset() - pic.evenOffset();
        sizeOdd = pic.rleSize() - pic.oddOffset();
    }
    else
    {
        sizeOdd = pic.evenOffset() - pic.oddOffset();
        sizeEven = pic.rleSize() - pic.evenOffset();
    }

    if (sizeEven <= 0 || sizeOdd <= 0)
    {
        throw QString("Corrupt buffer offset information");
    }

    for (int p = 0; p < pic.rleFragments.size(); ++p)
    {
        // copy data of all packet to one common buffer
        fragment = pic.rleFragments.at(p);
        for (int i = 0; i < fragment->imagePacketSize(); ++i)
        {
            buf.replace(index + i, (uchar)fileBuffer->getByte(fragment->imageBufferOffset() + i));
        }
        index += fragment->imagePacketSize();
    }

    decodeLine(buf, pic.evenOffset(), sizeEven, bm.image(), 0, width,  width * ((height / 2) + (height & 1)));
    decodeLine(buf, pic.oddOffset(), sizeOdd, bm.image(), width + (bm.image().bytesPerLine() - width), width, (height / 2) * width);

    if (warnings > 0)
    {
        subtitleProcessor->printWarning(QString("problems during RLE decoding of picture at offset %1\n")
                                        .arg(QString::number(startOfs, 16), 8, QChar('0')));
    }
    return bm;
}

void SubstreamDVD::decode(SubPictureDVD &pic, SubtitleProcessor* subtitleProcessor)
{
    palette = decodePalette(pic, srcPalette, subtitleProcessor->getAlphaCrop());
    bitmap = decodeImage(pic, palette.transparentIndex());

    QRect bounds = bitmap.bounds(palette, subtitleProcessor->getAlphaCrop());
    if (bounds.topLeft().y() > 0 || bounds.topLeft().x() > 0 ||
        bounds.bottomRight().x() < (bitmap.width() - 1) ||
        bounds.bottomRight().y() < (bitmap.height() - 1))
    {
        int width = bounds.width();
        int height = bounds.height();

        if (width < 2)
        {
            width = 2;
        }
        if (height < 2)
        {
            height = 2;
        }
        bitmap = bitmap.crop(bounds.topLeft().x(), bounds.topLeft().y(), width, height);
        pic.setImageWidth(width);
        pic.setImageHeight(height);
        pic.setOfsX(pic.originalX() + bounds.topLeft().x());
        pic.setOfsY(pic.originalY() + bounds.topLeft().y());
    }

    primaryColorIndex = bitmap.primaryColorIndex(palette, subtitleProcessor->getAlphaThreshold());
}

void SubstreamDVD::decodeLine(QVector<uchar> src, int srcOfs, int srcLen,
                              QImage &trg, int trgOfs, int width, int maxPixels)
{
    QVector<uchar> nibbles(srcLen * 2);
    int b;

    for (int i = 0; i < srcLen; ++i)
    {
        b = src[srcOfs + i] & 0xff;
        nibbles.replace(2 * i, (uchar)(b >> 4));
        nibbles.replace((2 * i) + 1, (uchar)(b & 0x0f));
    }

    int index = 0;
    int sumPixels = 0;
    int x = 0;

    while (index < nibbles.size() && sumPixels < maxPixels)
    {
        int len;
        int col;
        b = nibbles[index++] & 0xff;
        if (b == 0)
        {
            // three or four nibble code
            b = nibbles[index++] & 0xff;
            if ((b & 0xc) != 0)
            {
                // three byte code
                len = b << 2;
                b = nibbles[index++] & 0xff;
                len |= (b >> 2);
            }
            else
            {
                // line feed or four nibble code
                len = b << 6;
                b = nibbles[index++] & 0xff;
                len |= (b << 2);
                b = nibbles[index++] & 0xff;
                len |= (b >> 2);
                if (len == 0)
                {
                    // line feed
                    len = width - x;
                    if (len <= 0 || sumPixels >= maxPixels)
                    {
                        len = 0;
                        // handle line feed
                        trgOfs += 2 * width; // lines are interlaced!
                        sumPixels = ((trgOfs / width) / 2) * width;
                        x = 0;
                    }
                    if ((index & 1) == 1)
                    {
                        ++index;
                    }
                }
            }
        }
        else
        {
            // one or two nibble code
            len = b >> 2;
            if (len == 0)
            {
                // two nibble code
                len = b << 2;
                b = nibbles[index++] & 0xff;
                len |= (b >> 2);
            }
        }

        col = b & 0x3;
        sumPixels += len;


        uchar* pixels = trg.bits();
        for (int i = 0; i < len; ++i)
        {
            pixels[trgOfs + x] = (uchar)col;
            if (++x >= width)
            {
                trgOfs += (2 * (width + (trg.bytesPerLine() - width))); // lines are interlaced!
                x = 0;
                if ((index & 1) == 1)
                {
                    index++;
                }
            }
        }
    }
}
