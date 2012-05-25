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

#include "substreamdvd.h"
#include "palette.h"
#include "subpicturedvd.h"
#include "Tools/filebuffer.h"
#include "bitmap.h"
#include "types.h"
#include "subtitleprocessor.h"

#include <QImage>

SubstreamDVD::SubstreamDVD()
{
    srcPalette = new Palette(defaultPalR, defaultPalG, defaultPalB, defaultAlpha, true);
}

Palette *SubstreamDVD::decodePalette(SubPictureDVD *pic, Palette *palette, int alphaCrop)
{
    Palette* miniPalette = new Palette(4, true);

    for (int i = 0; i < miniPalette->getSize(); ++i)
    {
        int a = (pic->alpha[i] * 0xff) / 0xf;
        if (a >= alphaCrop)
        {
            miniPalette->setRGB(i, palette->getR()[pic->pal[i]], palette->getG()[pic->pal[i]], palette->getB()[pic->pal[i]]);
            miniPalette->setAlpha(i, a);
        }
        else
        {
            miniPalette->setARGB(i, 0);
        }
    }
    return miniPalette;
}

Bitmap *SubstreamDVD::decodeImage(SubPictureDVD *pic, int transIdx)
{
    int width = pic->originalWidth;
    int height = pic->originalHeight;
    int warnings = 0;

    ImageObjectFragment* fragment = pic->rleFragments.at(0);
    long startOfs = fragment->imageBufferOfs;

    if (width > pic->width || height > pic->height)
    {
        //TODO: print warning
    }

    Bitmap* bm = new Bitmap(width, height, transIdx);

    QVector<uchar> buf(pic->rleSize);
    int index = 0;

    int sizeEven;
    int sizeOdd;

    if (pic->oddOfs > pic->evenOfs)
    {
        sizeEven = pic->oddOfs - pic->evenOfs;
        sizeOdd = pic->rleSize - pic->oddOfs;
    }
    else
    {
        sizeOdd = pic->evenOfs - pic->oddOfs;
        sizeEven = pic->rleSize - pic->evenOfs;
    }

    if (sizeEven <= 0 || sizeOdd <= 0)
    {
        //TODO: error handling
        throw 10;
    }

    for (int p = 0; p < pic->rleFragments.size(); ++p)
    {
        // copy data of all packet to one common buffer
        fragment = pic->rleFragments.at(p);
        for (int i = 0; i < fragment->imagePacketSize; ++i)
        {
            buf.replace(index + i, (uchar)fileBuffer->getByte(fragment->imageBufferOfs + i));
        }
        index += fragment->imagePacketSize;
    }

    decodeLine(buf, pic->evenOfs, sizeEven, bm->getImg(), 0, width,  width * ((height / 2) + (height & 1)));
    decodeLine(buf, pic->oddOfs, sizeOdd, bm->getImg(), width + (bm->getImg()->bytesPerLine() - width), width, (height / 2) * width);

    if (warnings > 0)
    {
        //TODO: print warnings;
    }
    return bm;
}

void SubstreamDVD::decode(SubPictureDVD *pic, SubtitleProcessor* subtitleProcessor)
{
    palette = decodePalette(pic, srcPalette, subtitleProcessor->getAlphaCrop());
    bitmap = decodeImage(pic, palette->getTransparentIndex());

    QRect bounds = bitmap->getBounds(palette, subtitleProcessor->getAlphaCrop());
    if (bounds.topLeft().y() > 0 || bounds.topLeft().x() > 0 ||
        bounds.bottomRight().x() < (bitmap->getWidth() - 1) ||
        bounds.bottomRight().y() < (bitmap->getHeight() - 1))
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
        bitmap = bitmap->crop(bounds.topLeft().x(), bounds.topLeft().y(), width, height);
        pic->setImageWidth(width);
        pic->setImageHeight(height);
        pic->setOfsX(pic->originalX + bounds.topLeft().x());
        pic->setOfsY(pic->originalY + bounds.topLeft().y());
    }

    primaryColorIndex = bitmap->getPrimaryColorIndex(palette, subtitleProcessor->getAlphaThreshold());
}

void SubstreamDVD::decodeLine(QVector<uchar> src, int srcOfs, int srcLen, QImage* trg, int trgOfs, int width, int maxPixels)
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


        uchar* pixels = trg->bits();
        for (int i = 0; i < len; ++i)
        {
            pixels[trgOfs + x] = (uchar)col;
            if (++x >= width)
            {
                trgOfs += (2 * (width + (trg->bytesPerLine() - width))); // lines are interlaced!
                x = 0;
                if ((index & 1) == 1)
                {
                    index++;
                }
            }
        }
    }
}
