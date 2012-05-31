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

#include "bitmap.h"
#include "palette.h"
#include "palettebitmap.h"

#include <QImage>
#include <QHash>
#include <vector>

Bitmap::Bitmap(Bitmap &other) :
    width(other.width),
    height(other.height),
    img(other.img)
{
}

Bitmap::Bitmap(Bitmap *other) :
    width(other->width),
    height(other->height),
    img(other->img)
{
}

Bitmap::Bitmap(int width, int height) :
    width(width),
    height(height)
{
    img = new QImage(width, height, QImage::Format_Indexed8);
}

Bitmap::Bitmap(int width, int height, int color) :
    width(width),
    height(height)
{
    img = new QImage(width, height, QImage::Format_Indexed8);
    clear(color);
}

QRect Bitmap::getBounds(Palette* palette, int alphaThreshold)
{
    QVector<uchar> a = palette->getAlpha();
    int xMin, xMax, yMin, yMax;

    yMax = img->height() - 1;
    for (int y = yMax; y > 0; --y, --yMax)
    {
        uchar * pixels = img->scanLine(y);
        for (int x = 0; x < img->width(); ++x)
        {
            int idx = pixels[x] & 0xff;
            int alpha = a[idx] & 0xff;
            if (alpha >= alphaThreshold)
            {
                goto loop1;
            }
        }
    }
    loop1:

    yMin = 0;
    for (int y = yMin; y < yMax; ++y, ++yMin)
    {
        uchar * pixels = img->scanLine(y);
        for (int x = 0; x < img->width(); x++)
        {
            int idx = pixels[x] & 0xff;
            if ((a[idx] & 0xff) >= alphaThreshold)
            {
                goto loop2;
            }
        }
    }
    loop2:

    xMax = img->width() - 1;
    for (int x = xMax; x > 0 ; --x, --xMax)
    {
        for (int y = yMin; y < yMax; y++)
        {
            uchar * pixels = img->scanLine(y);
            int idx = pixels[x] & 0xff;
            if ((a[idx] & 0xff) >= alphaThreshold)
            {
                goto loop3;
            }
        }
    }
    loop3:

    xMin = 0;
    for (int x = xMin; x < xMax; ++x, ++xMin)
    {
        for (int y = yMin; y < yMax; y++)
        {
            uchar * pixels = img->scanLine(y);
            int idx = pixels[x] & 0xff;
            if ((a[idx] & 0xff) >= alphaThreshold)
            {
                goto loop4;
            }
        }
    }
    loop4:

    return QRect(QPoint(xMin, yMin), QPoint(xMax, yMax));
}

void Bitmap::clear(int color)
{
    img->fill(color);
}

Bitmap* Bitmap::crop(int x, int y, int width, int height)
{
    Bitmap* bm = new Bitmap(width, height);

    bm->img = new QImage(img->copy(x, y, width, height));

    return bm;
}

int Bitmap::getPrimaryColorIndex(Palette* palette, int alphaThreshold)
{
    QVector<int> histogram(palette->getSize(), 0);

    for (int i = 0; i < img->height(); ++i)
    {
        uchar* pixels = img->scanLine(i);
        for (int j = 0; j < img->width(); ++j)
        {
            int oldValue = histogram.at(pixels[j] & 0xff);
            histogram.replace(pixels[j] & 0xff, ++oldValue);
        }
    }

    for (int i = 0; i < histogram.size(); ++i)
    {
        int alpha = palette->getAlpha(i);
        if (alpha < alphaThreshold)
        {
            alpha = 0;
        }
        histogram.replace(i, ((histogram.at(i) * alpha) + 128) / 256);
        histogram.replace(i, ((histogram.at(i) * ((palette->getY()[i] & 0xff))) + 128) / 256);
    }

    int max = 0;
    int color = 0;

    for (int i = 0; i < histogram.size(); ++i)
    {
        if (histogram.at(i) > max)
        {
            max = histogram.at(i);
            color = i;
        }
    }
    return color;
}

QImage *Bitmap::getImage(Palette* palette)
{
    QImage* newImage = new QImage(img->bits(), img->width(), img->height(), img->format());
    newImage->setColorTable(palette->getColorTable());
    return newImage;
}

Bitmap *Bitmap::convertLm(Palette *palette, int alphaThreshold, QVector<int> lumaThreshold)
{
    QVector<uchar> cy = palette->getY();
    QVector<uchar> a = palette->getAlpha();
    Bitmap* bm = new Bitmap(width, height);
    QImage* destImage = bm->getImg();

    // select nearest colors in existing palette
    QHash<int, int> p;

    for (int i = 0; i < img->height(); ++i)
    {
        uchar* sourcePixels = img->scanLine(i);
        uchar* destPixels = destImage->scanLine(i);
        for (int j = 0; j < img->width(); ++j)
        {
            int colIdx;
            int idx = sourcePixels[j] & 0xff;
            int alpha = a.at(idx) & 0xff;
            int cyp   = cy.at(idx) & 0xff;

            int key = ((alpha << 8) | cyp);
            if (p.contains(key))
            {
                colIdx = p[key];
            }
            else
            {
                colIdx = 0;

                // determine index in target
                if (alpha < alphaThreshold)
                {
                    colIdx = 0; // transparent color
                }
                else
                {
                    colIdx = 1; // default: lightest color
                    for (int n = 0; n < lumaThreshold.size(); ++n)
                    {
                        if (cyp > lumaThreshold.at(n))
                        {
                            break;
                        }
                        ++colIdx; // try next darker color
                    }
                }
                p.insert(key, colIdx);
            }
            // write target pixel
            destPixels[j] = (uchar) colIdx;
        }
    }
    return bm;
}

Bitmap *Bitmap::scaleFilter(int sizeX, int sizeY, Palette *palette, Filter *filter)
{
    //TODO: finish implementing
    throw 10;
}

PaletteBitmap *Bitmap::scaleFilter(int sizeX, int sizeY, Palette *palette, Filter *filter, bool dither)
{
    //TODO: finish implementing
    throw 10;
}

Bitmap *Bitmap::scaleFilterLm(int sizeX, int sizeY, Palette *palette, int alphaThreshold, QVector<int> lumaThreshold, Filter *filter)
{
    //TODO: finish implementing
    throw 10;
}

Bitmap *Bitmap::scaleBilinear(int sizeX, int sizeY, Palette *palette)
{
    //TODO: finish implementing
    throw 10;
}

PaletteBitmap *Bitmap::scaleBilinear(int sizeX, int sizeY, Palette *palette, bool dither)
{
    //TODO: finish implementing
    throw 10;
}

Bitmap *Bitmap::scaleBilinearLm(int sizeX, int sizeY, Palette *palette, int alphaThreshold, QVector<int> lumaThreshold)
{
    //TODO: finish implementing
    throw 10;
}

void Bitmap::fillRect(int x, int y, int width, int height, int color)
{
    uchar b = (uchar)color;

    for (int i = y; i < height; ++i)
    {
        uchar* pixels = img->scanLine(i);
        for (int j = x; j < width; ++j)
        {
            pixels[j] = b;
        }
    }
}

QImage *Bitmap::getImg()
{
    return img;
}

void Bitmap::setImg(QImage* newImage)
{
    img = newImage;
}

int Bitmap::getHighestColorIndex(Palette *pal)
{
    // create histogram for palette
    int maxIdx = 0;
    for (int y = 0; y < img->height(); ++y)
    {
        uchar* pixels = img->scanLine(y);
        for (int x = 0; x < img->width(); ++x)
        {
            int idx = pixels[x] & 0xff;
            if (pal->getAlpha(idx) > 0)
            {
                if (idx > maxIdx)
                {
                    maxIdx = idx;
                    if (maxIdx == 255)
                    {
                        goto breakOut;
                    }
                }
            }
        }
    }
    breakOut:
    return maxIdx;
}

QImage *Bitmap::toARGB(Palette *pal)
{
    QImage* newImage = new QImage(img->width(), img->height(), QImage::Format_ARGB32);
    for (int y = 0; y < newImage->height(); ++y)
    {
        uchar* pixels = img->scanLine(y);
        QRgb* newPixels = (QRgb*)newImage->scanLine(y);
        for (int x = 0; x < newImage->width(); ++x)
        {
            newPixels[x] = (uint)pal->getARGB(pixels[x] && 0xff);
        }
    }
}
