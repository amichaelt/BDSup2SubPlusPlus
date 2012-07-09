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

#include "bitmap.h"
#include "palette.h"
#include "palettebitmap.h"
#include "Tools/quantizefilter.h"
#include "Filters/filterop.h"

#include <QImage>
#include <QHash>
#include <QFile>
#include <QRect>
#include <QVector>

Bitmap::Bitmap()
{
}

Bitmap::Bitmap(Bitmap &other) :
    width(other.width),
    height(other.height)
{
    img = other.img;
}

Bitmap::Bitmap(Bitmap *other) :
    width(other->width),
    height(other->height)
{
    img = other->img;
}

Bitmap::Bitmap(int width, int height) :
    width(width),
    height(height),
    img(width, height, QImage::Format_Indexed8)
{
}

Bitmap::Bitmap(int width, int height, int color) :
    width(width),
    height(height),
    img(width, height, QImage::Format_Indexed8)
{
    clear(color);
}

Bitmap::Bitmap(int width, int height, QImage image) :
    width(width),
    height(height),
    img(image)
{
}

Bitmap::~Bitmap()
{
}

const QRect &Bitmap::getBounds(Palette &palette, int alphaThreshold)
{
    QVector<QRgb> a = palette.getColorTable();
    int xMin, xMax, yMin, yMax;

    yMax = img.height() - 1;
    for (int y = yMax; y > 0; --y, --yMax)
    {
        uchar * pixels = img.scanLine(y);
        for (int x = 0; x < img.width(); ++x)
        {
            int idx = pixels[x] & 0xff;
            int alpha = qAlpha(a[idx]);
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
        uchar * pixels = img.scanLine(y);
        for (int x = 0; x < img.width(); x++)
        {
            int idx = pixels[x] & 0xff;
            if (qAlpha(a[idx]) >= alphaThreshold)
            {
                goto loop2;
            }
        }
    }
    loop2:

    xMax = img.width() - 1;
    for (int x = xMax; x > 0 ; --x, --xMax)
    {
        for (int y = yMin; y < yMax; y++)
        {
            uchar * pixels = img.scanLine(y);
            int idx = pixels[x] & 0xff;
            if (qAlpha(a[idx]) >= alphaThreshold)
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
            uchar * pixels = img.scanLine(y);
            int idx = pixels[x] & 0xff;
            if (qAlpha(a[idx]) >= alphaThreshold)
            {
                goto loop4;
            }
        }
    }
    loop4:

    const QRect &rect = QRect(QPoint(xMin, yMin), QPoint(xMax, yMax));

    return rect;
}

void Bitmap::clear(int color)
{
    img.fill(color);
}

Bitmap Bitmap::crop(int x, int y, int width, int height)
{
    Bitmap bm(width, height, img.copy(x, y, width, height));

    return bm;
}

int Bitmap::getPrimaryColorIndex(Palette &palette, int alphaThreshold)
{
    QVector<int> histogram(palette.getSize(), 0);

    for (int i = 0; i < img.height(); ++i)
    {
        uchar* pixels = img.scanLine(i);
        for (int j = 0; j < img.width(); ++j)
        {
            int oldValue = histogram.at(pixels[j] & 0xff);
            histogram.replace(pixels[j] & 0xff, ++oldValue);
        }
    }

    for (int i = 0; i < histogram.size(); ++i)
    {
        int alpha = palette.getAlpha(i);
        if (alpha < alphaThreshold)
        {
            alpha = 0;
        }
        histogram.replace(i, ((histogram.at(i) * alpha) + 128) / 256);
        histogram.replace(i, ((histogram.at(i) * ((palette.getY()[i] & 0xff))) + 128) / 256);
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

QImage Bitmap::getImage(Palette &palette)
{
    QImage newImage(img.bits(), img.width(), img.height(), img.format());
    newImage.setColorTable(palette.getColorTable());
    return newImage;
}

Bitmap Bitmap::convertLm(Palette &palette, int alphaThreshold, QVector<int>& lumaThreshold)
{
    QVector<uchar> cy = palette.getY();
    QVector<QRgb> a = palette.getColorTable();
    Bitmap bm(width, height);
    QImage &destImage = bm.getImg();

    // select nearest colors in existing palette
    QHash<int, int> p;

    for (int i = 0; i < img.height(); ++i)
    {
        uchar* sourcePixels = img.scanLine(i);
        uchar* destPixels = destImage.scanLine(i);
        for (int j = 0; j < img.width(); ++j)
        {
            int colIdx;
            int idx = sourcePixels[j] & 0xff;
            int alpha = qAlpha(a[idx]);
            int cyp = cy.at(idx) & 0xff;

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

Bitmap Bitmap::scaleFilter(int sizeX, int sizeY, Palette &palette, Filter &filter)
{
    QVector<QRgb> rgb = palette.getColorTable();

    FilterOp filterOp(filter);
    QVector<QRgb> trg = filterOp.filter(*this, palette, sizeX, sizeY);

    Bitmap bm(sizeX, sizeY);

    // select nearest colors in existing palette
    QHash<QRgb, int> p;
    int offset = 0;

    QImage image(bm.getImg().bits(), bm.getImg().width(), bm.getImg().height(), bm.getImg().format());
    image.setColorTable(palette.getColorTable());

    for (int y = 0; y < bm.getImg().height(); ++y)
    {
        uchar* pixels = image.scanLine(y);
        for (int x = 0; x < bm.getImg().width(); ++x)
        {
            offset = (y * sizeX) + x;
            QRgb color = trg[offset];
            int colIdx;
            if (p.contains(color))
            {
                colIdx = p.value(color);
            }
            else
            {
                colIdx = 0;
                int minDistance = 0xffffff; // init > 0xff*0xff*4 = 0x03f804
                int alpha = qAlpha(color);
                int red = qRed(color);
                int green = qGreen(color);
                int blue = qBlue(color);
                for (int idx = 0; idx < palette.getSize(); ++idx)
                {
                    // distance vector (skip sqrt)
                    int ad = alpha - qAlpha(rgb[idx]);
                    int rd = red - qRed(rgb[idx]);
                    int gd = green - qGreen(rgb[idx]);
                    int bd = blue - qBlue(rgb[idx]);
                    int distance = (rd * rd) + (gd * gd) + (bd * bd) + (ad * ad);
                    // new minimum distance ?
                    if (distance < minDistance)
                    {
                        colIdx = idx;
                        minDistance = distance;
                        if (minDistance == 0)
                        {
                            break;
                        }
                    }
                }
                p.insert(color, colIdx);
            }
            // write target pixel
            pixels[x] = (uchar)colIdx;
        }
    }
    return bm;
}

PaletteBitmap Bitmap::scaleFilter(int sizeX, int sizeY, Palette &palette, Filter &filter, bool dither)
{
    FilterOp fOp(filter);
    QVector<QRgb> trgPixels = fOp.filter(*this, palette, sizeX, sizeY);

    QImage trg(sizeX, sizeY, QImage::Format_ARGB32);
    int offset = 0;
    for (int y = 0; y < trg.height(); ++y)
    {
        QRgb* pixels = (QRgb*)trg.scanLine(y);
        for (int x = 0; x < trg.width(); ++x)
        {
            offset = (y * sizeX) + x;
            pixels[x] = trgPixels[offset];
        }
    }

    // quantize image
    QuantizeFilter qf;
    Bitmap bm(sizeX, sizeY);
    QVector<QRgb> ct = qf.quantize(trg, &bm.getImg(), sizeX, sizeY, 255, dither, dither);
    int size = ct.size();
    if (size > 255)
    {
        size = 255;
    }
    // create palette
    Palette* trgPal = new Palette(256);
    for (int i = 0; i < size; ++i)
    {
        trgPal->setARGB(i,ct[i]);
    }

    PaletteBitmap bitmap(bm, *trgPal);
    return bitmap;
}

Bitmap Bitmap::scaleFilterLm(int sizeX, int sizeY, Palette &palette, int alphaThreshold, QVector<int> &lumaThreshold, Filter &filter)
{
    FilterOp filterOp(filter);
    QVector<QRgb> trg = filterOp.filter(*this, palette, sizeX, sizeY);

    Bitmap bm(sizeX, sizeY);

    // select nearest colors in existing palette
    QHash<QRgb, int> p;
    int offset = 0;
    QImage image(bm.getImg().bits(), bm.getImg().width(), bm.getImg().height(), bm.getImg().format());
    image.setColorTable(palette.getColorTable());

    for (int y = 0; y < bm.getImg().height(); ++y)
    {
        uchar* pixels = image.scanLine(y);
        for (int x = 0; x < bm.getImg().width(); ++x)
        {
            offset = (y * sizeX) + x;
            QRgb color = trg[offset];
            int colIdx;
            if (p.contains(color))
            {
                colIdx = p.value(color);
            }
            else
            {
                colIdx = 0;
                int alpha = qAlpha(color);
                int red = qRed(color);
                int green = qGreen(color);
                int blue = qBlue(color);
                int cyp = palette.RGB2YCbCr(qRgb(red, green, blue), false)[0];

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
                        if (cyp > lumaThreshold[n])
                        {
                            break;
                        }
                        colIdx++; // try next darker color
                    }
                }
                p.insert(color, colIdx);
            }
            // write target pixel
            pixels[x] = (uchar)colIdx;
        }
    }
    return bm;
}

Bitmap Bitmap::scaleBilinear(int sizeX, int sizeY, Palette &palette)
{
    QVector<QRgb> rgb = palette.getColorTable();

    double scaleX = (double)(width - 1) / (sizeX - 1);
    double scaleY = (double)(height - 1) / (sizeY - 1);

    int lastR = 0;
    int lastG = 0;
    int lastB = 0;
    int lastA = 0;
    int lastColIdx = palette.getTransparentIndex();

    Bitmap trg(sizeX, sizeY);

    for (int yt = 0; yt < sizeY; ++yt)
    {
        double ys = yt * scaleY;     // source coordinate
        int ysi = (int)ys;
        double wy = (ys - ysi);
        double wy1 = 1.0 - wy;

        for (int xt = 0; xt < sizeX; ++xt)
        {
            double xs = xt * scaleX;     // source coordinate
            int xsi = (int)xs;
            double wx = (xs - xsi);	// weight factor

            double wx1 = 1.0 - wx;

            // top left
            double w = wx1 * wy1;
            int idx = img.scanLine(ysi)[xsi] & 0xff;
            double at = qAlpha(rgb[idx]) * w;
            double rt = qRed(rgb[idx]) * w;
            double gt = qGreen(rgb[idx]) * w;
            double bt = qBlue(rgb[idx]) * w;

            // top right
            if (xsi < width - 1)
            {
                w = wx * wy1;
                idx = img.scanLine(ysi)[xsi + 1] & 0xff;
                at += qAlpha(rgb[idx]) * w;
                rt += qRed(rgb[idx]) * w;
                gt += qGreen(rgb[idx]) * w;
                bt += qBlue(rgb[idx]) * w;
            }  // else assume transparent black

            // bottom left
            if (ysi < height - 1)
            {
                w = wx1 * wy;
                idx = img.scanLine(ysi + 1)[xsi] & 0xff;
                at += qAlpha(rgb[idx]) * w;
                rt += qRed(rgb[idx]) * w;
                gt += qGreen(rgb[idx]) * w;
                bt += qBlue(rgb[idx]) * w;
            } // else assume transparent black

            // bottom right
            if ((ysi < height - 1) && (xsi < width - 1))
            {
                w = wx * wy;
                idx = img.scanLine(ysi + 1)[xsi + 1] & 0xff;
                at += qAlpha(rgb[idx]) * w;
                rt += qRed(rgb[idx]) * w;
                gt += qGreen(rgb[idx]) * w;
                bt += qBlue(rgb[idx]) * w;

            } // else assume transparent black

            int ati = (int)(at + 0.5);
            int rti = (int)(rt + 0.5);
            int gti = (int)(gt + 0.5);
            int bti = (int)(bt + 0.5);

            // find color index in palette

            // already known ?
            int colIdx = lastColIdx;
            if (ati != lastA || rti != lastR || gti != lastG || bti!=lastB)
            {
                int minDistance = 0xffffff; // init > 0xff*0xff*4 = 0x03f804
                for (idx = 0; idx < palette.getSize(); ++idx)
                {
                    // distance vector (skip sqrt)
                    int ad = ati - qAlpha(rgb[idx]);
                    int rd = rti - qRed(rgb[idx]);
                    int gd = gti - qGreen(rgb[idx]);
                    int bd = bti - qBlue(rgb[idx]);
                    int distance = (rd * rd) + (gd * gd) + (bd * bd) + (ad * ad);
                    // new minimum distance ?
                    if (distance < minDistance)
                    {
                        colIdx = idx;
                        minDistance = distance;
                        if (minDistance == 0)
                        {
                            break;
                        }
                    }
                }
                // remember values
                lastA = ati;
                lastR = rti;
                lastG = gti;
                lastB = bti;
                lastColIdx = colIdx;
            }
            // write target pixel
            trg.getImg().scanLine(yt)[xt] = (uchar)colIdx;
        }
    }
    return trg;
}

PaletteBitmap Bitmap::scaleBilinear(int sizeX, int sizeY, Palette &palette, bool dither)
{
    QVector<QRgb> rgb = palette.getColorTable();

    double scaleX = (double)(width - 1) / (sizeX - 1);
    double scaleY = (double)(height - 1) / (sizeY - 1);

    QImage trg(sizeX, sizeY, QImage::Format_ARGB32);

    for (int yt = 0; yt < sizeY; ++yt)
    {
        double ys = yt * scaleY;     // source coordinate
        int ysi = (int)ys;
        double wy = (ys - ysi);
        double wy1 = 1.0 - wy;

        for (int xt = 0; xt < sizeX; ++xt)
        {
            double xs = xt * scaleX;     // source coordinate
            int xsi = (int)xs;
            double wx = (xs - xsi);      // weight factor

            int idx;

            // interpolate pixel
            double wx1 = 1.0 - wx;
            int x, y;

            // top left
            double w = wx1 * wy1;
            idx = img.scanLine(ysi)[xsi] & 0xff;
            double at = qAlpha(rgb[idx]) * w;
            double rt = qRed(rgb[idx]) * w;
            double gt = qGreen(rgb[idx]) * w;
            double bt = qBlue(rgb[idx]) * w;

            // top right
            x = xsi + 1;
            if (x < width)
            {
                w = wx * wy1;
                idx = img.scanLine(ysi)[x] & 0xff;
                at += qAlpha(rgb[idx]) * w;
                rt += qRed(rgb[idx]) * w;
                gt += qGreen(rgb[idx]) * w;
                bt += qBlue(rgb[idx]) * w;
            }  // else assume transparent black

            // bottom left
            y = ysi + 1;
            if (y < height)
            {
                w = wx1 * wy;
                idx = img.scanLine(y)[xsi] & 0xff;
                at += qAlpha(rgb[idx]) * w;
                rt += qRed(rgb[idx]) * w;
                gt += qGreen(rgb[idx]) * w;
                bt += qBlue(rgb[idx]) * w;
            } // else assume transparent black

            // bottom right
            x = xsi + 1;
            y = ysi + 1;
            if ((x < width) && (y < height))
            {
                w = wx * wy;
                idx = img.scanLine(y)[x] & 0xff;
                at += qAlpha(rgb[idx]) * w;
                rt += qRed(rgb[idx]) * w;
                gt += qGreen(rgb[idx]) * w;
                bt += qBlue(rgb[idx]) * w;
            } // else assume transparent black

            int ati = (int)(at);
            int rti = (int)(rt);
            int gti = (int)(gt);
            int bti = (int)(bt);

            QRgb* pixels = (QRgb*)trg.scanLine(yt);
            pixels[xt] = qRgba(rti, gti, bti, ati);;
        }
    }
    // quantize image
    QuantizeFilter qf;
    Bitmap bm(sizeX, sizeY, QImage::Format_Indexed8);
    QVector<QRgb> ct = qf.quantize(trg, &bm.getImg(), sizeX, sizeY, 255, dither, dither);
    int size = ct.size();
    if (size > 255)
    {
        size = 255;
    }
    // create palette
    Palette* trgPal = new Palette(256);
    for (int i = 0; i < size; ++i)
    {
        trgPal->setARGB(i, ct[i]);
    }
    PaletteBitmap bitmap(bm, *trgPal);

    return bitmap;
}

Bitmap Bitmap::scaleBilinearLm(int sizeX, int sizeY, Palette &palette, int alphaThreshold, QVector<int> &lumaThreshold)
{
    QVector<uchar> cy = palette.getY();
    QVector<QRgb> a = palette.getColorTable();

    double scaleX = (double)(width - 1) / (sizeX - 1);
    double scaleY = (double)(height - 1) / (sizeY - 1);

    int lastCY = 0;
    int lastA  = 0;
    int lastColIdx = 0; // 0 is the transparent color

    Bitmap trg(sizeX, sizeY);

    for (int yt = 0; yt < sizeY; ++yt)
    {
        double ys = yt * scaleY;     // source coordinate
        int ysi = (int)ys;
        double wy = (ys - ysi);
        double wy1 = 1.0 - wy;

        for (int xt = 0; xt < sizeX; ++xt)
        {
            double xs = xt * scaleX;     // source coordinate
            int xsi = (int)xs;
            double wx = (xs - xsi);	// weight factor

            double wx1 = 1.0 - wx;

            // interpolate pixel

            // top left
            double w = wx1 * wy1;
            int idx = img.scanLine(ysi)[xsi] & 0xff;
            double at = qAlpha(a[idx]) * w;
            double cyt = (cy[idx] & 0xff) * w;

            // top right
            if (xsi < width - 1)
            {
                w = wx * wy1;
                idx = img.scanLine(ysi)[xsi + 1] & 0xff;
                at += qAlpha(a[idx]) * w;
                cyt += (cy[idx] & 0xff) * w;
            }  // else assume transparent black

            // bottom left
            if (ysi < height - 1)
            {
                w = wx1 * wy;
                idx = img.scanLine(ysi + 1)[xsi] & 0xff;
                at += qAlpha(a[idx]) * w;
                cyt += (cy[idx] & 0xff) * w;
            } // else assume transparent black

            // bottom right
            if ((ysi < height - 1) && (xsi < width - 1))
            {
                w = wx * wy;
                idx = img.scanLine(ysi + 1)[xsi + 1] & 0xff;
                at += qAlpha(a[idx]) * w;
                cyt += (cy[idx] & 0xff) * w;
            } // else assume transparent black

            int ati = (int)at;
            int cyti = (int)cyt;

            // find color index in palette

            // already known ?
            int colIdx = lastColIdx;
            if (ati != lastA || cyti != lastCY)
            {
                // determine index in target
                if (ati < alphaThreshold)
                {
                    colIdx = 0; // transparent color
                }
                else
                {
                    colIdx = 1; // default: lightest color
                    for (int n = 0; n < lumaThreshold.size(); ++n)
                    {
                        if (cyti > lumaThreshold[n])
                        {
                            break;
                        }
                        colIdx++; // try next darker color
                    }
                }
                // remember
                lastA = ati;
                lastCY = cyti;
                lastColIdx = colIdx;
            }
            // write target pixel
            trg.getImg().scanLine(yt)[xt] = (uchar)colIdx;
        }
    }
    return trg;
}

void Bitmap::fillRect(int x, int y, int width, int height, int color)
{
    uchar b = (uchar)color;

    for (int i = y; i < (y + height); ++i)
    {
        uchar* pixels = img.scanLine(i);
        for (int j = x; j < (x + width); ++j)
        {
            pixels[j] = b;
        }
    }
}

QImage &Bitmap::getImg()
{
    return img;
}

void Bitmap::setImg(QImage &newImage)
{
    img = newImage;
}

int Bitmap::getHighestColorIndex(Palette &palette)
{
    // create histogram for palette
    int maxIdx = 0;
    for (int y = 0; y < img.height(); ++y)
    {
        uchar* pixels = img.scanLine(y);
        for (int x = 0; x < img.width(); ++x)
        {
            int idx = pixels[x] & 0xff;
            if (palette.getAlpha(idx) > 0)
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

QImage Bitmap::toARGB(Palette &palette)
{
    QImage newImage(img.width(), img.height(), QImage::Format_ARGB32);
    for (int y = 0; y < newImage.height(); ++y)
    {
        uchar* pixels = img.scanLine(y);
        QRgb* newPixels = (QRgb*)newImage.scanLine(y);
        for (int x = 0; x < newImage.width(); ++x)
        {
            newPixels[x] = palette.getARGB(pixels[x]);
        }
    }
    return newImage;
}
