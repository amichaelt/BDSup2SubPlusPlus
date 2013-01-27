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
    subtitleImage(other.subtitleImage.copy())
{
}

Bitmap::Bitmap(Bitmap *other) :
    subtitleImage(other->subtitleImage.copy())
{
}

Bitmap::Bitmap(int width, int height) :
    subtitleImage(width, height, QImage::Format_Indexed8)
{
}

Bitmap::Bitmap(int width, int height, int color) :
    subtitleImage(width, height, QImage::Format_Indexed8)
{
    clear(color);
}

Bitmap::Bitmap(QImage image) :
    subtitleImage(image)
{
}

QRect Bitmap::bounds(Palette &palette, int alphaThreshold)
{
    QVector<QRgb> a = palette.colorTable();
    int xMin, xMax, yMin, yMax;

    yMax = subtitleImage.height() - 1;
    for (int y = yMax; y > 0; --y, --yMax)
    {
        uchar * pixels = subtitleImage.scanLine(y);
        for (int x = 0; x < subtitleImage.width(); ++x)
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
        uchar * pixels = subtitleImage.scanLine(y);
        for (int x = 0; x < subtitleImage.width(); x++)
        {
            int idx = pixels[x] & 0xff;
            if (qAlpha(a[idx]) >= alphaThreshold)
            {
                goto loop2;
            }
        }
    }
    loop2:

    xMax = subtitleImage.width() - 1;
    for (int x = xMax; x > 0 ; --x, --xMax)
    {
        for (int y = yMin; y < yMax; y++)
        {
            uchar * pixels = subtitleImage.scanLine(y);
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
            uchar * pixels = subtitleImage.scanLine(y);
            int idx = pixels[x] & 0xff;
            if (qAlpha(a[idx]) >= alphaThreshold)
            {
                goto loop4;
            }
        }
    }
    loop4:

    QRect rect = QRect(QPoint(xMin, yMin), QPoint(xMax, yMax));

    return rect;
}

void Bitmap::clear(int color)
{
    subtitleImage.fill(color);
}

Bitmap Bitmap::crop(int x1, int y1, int width, int height)
{
    Bitmap bm(subtitleImage.copy(x1, y1, width, height));

    return bm;
}

int Bitmap::primaryColorIndex(Palette &palette, int alphaThreshold)
{
    int *histogram = (int*) calloc(palette.size(), sizeof(int));
    int height = subtitleImage.height();
    int width = subtitleImage.width();
    uchar* pixels = subtitleImage.scanLine(0);
    int scanWidth = subtitleImage.bytesPerLine();

    for (int i = 0; i < height; ++i)
    {
        for (int j = 0; j < width; ++j)
        {
            int oldValue = histogram[pixels[j] & 0xff];
            histogram[pixels[j] & 0xff] = ++oldValue;
        }
        pixels += scanWidth;
    }

    const uchar *Y = palette.Y().constData();
    for (int i = 0; i < palette.size(); ++i)
    {
        int alpha = palette.alpha(i);
        if (alpha < alphaThreshold)
        {
            alpha = 0;
        }
        histogram[i] = ((histogram[i] * alpha) + 128) / 256;
        histogram[i] = ((histogram[i] * ((Y[i] & 0xff))) + 128) / 256;
    }

    int max = 0;
    int color = 0;

    for (int i = 0; i < palette.size(); ++i)
    {
        if (histogram[i] > max)
        {
            max = histogram[i];
            color = i;
        }
    }
    free(histogram);
    return color;
}

Bitmap Bitmap::convertLm(Palette &palette, int alphaThreshold, QVector<int>& lumaThreshold)
{
    QVector<uchar> cy = palette.Y();
    QVector<QRgb> a = palette.colorTable();

    int height = subtitleImage.height();
    int width = subtitleImage.width();

    Bitmap bm(width, height);

    uchar* sourcePixels = subtitleImage.scanLine(0);
    int sourcePitch = subtitleImage.bytesPerLine();

    QImage &destImage = bm.image();

    uchar *destPixels = destImage.scanLine(0);
    int destPitch = destImage.bytesPerLine();

    // select nearest colors in existing palette
    QHash<int, int> p;

    for (int i = 0; i < height; ++i)
    {
        uchar* sourcePixels = subtitleImage.scanLine(i);
        uchar* destPixels = destImage.scanLine(i);
        for (int j = 0; j < width; ++j)
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
        sourcePixels += sourcePitch;
        destPixels += destPitch;
    }
    return bm;
}

Bitmap Bitmap::scaleFilter(int sizeX, int sizeY, Palette &palette, Filter &filter)
{
    QVector<QRgb> rgb = palette.colorTable();

    FilterOp filterOp(filter);
    QVector<QRgb> trg = filterOp.filter(*this, palette, sizeX, sizeY);

    Bitmap bm(sizeX, sizeY);

    // select nearest colors in existing palette
    QHash<QRgb, int> p;
    int offset = 0;

    int width = bm.image().width();
    int height = bm.image().height();

    QImage image(bm.image().bits(), width, height, bm.image().format());
    image.setColorTable(palette.colorTable());

    uchar* pixels = image.scanLine(0);
    int sourcePitch = image.bytesPerLine();

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
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
                for (int idx = 0; idx < palette.size(); ++idx)
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
        pixels += sourcePitch;
    }
    return bm;
}

PaletteBitmap Bitmap::scaleFilter(int sizeX, int sizeY, Palette &palette, Filter &filter, bool dither)
{
    FilterOp fOp(filter);
    QVector<QRgb> trgPixels = fOp.filter(*this, palette, sizeX, sizeY);

    QImage trg(sizeX, sizeY, QImage::Format_ARGB32);
    int offset = 0;

    QRgb* pixels = (QRgb*)trg.scanLine(0);
    int sourcePitch = trg.bytesPerLine() / 4;

    for (int y = 0; y < sizeY; ++y)
    {
        for (int x = 0; x < sizeX; ++x)
        {
            offset = (y * sizeX) + x;
            pixels[x] = trgPixels[offset];
        }
        pixels += sourcePitch;
    }

    // quantize image
    QuantizeFilter qf;
    Bitmap bm(sizeX, sizeY);
    QVector<QRgb> ct = qf.quantize(trg, &bm.image(), sizeX, sizeY, 255, dither, dither);
    int size = ct.size();
    if (size > 255)
    {
        size = 255;
    }
    // create palette
    Palette trgPal(256);
    for (int i = 0; i < size; ++i)
    {
        trgPal.setARGB(i, ct[i]);
    }

    PaletteBitmap bitmap(bm, trgPal);
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

    QImage image(bm.image().bits(), bm.image().width(), bm.image().height(), bm.image().format());
    image.setColorTable(palette.colorTable());

    uchar* pixels = image.scanLine(0);
    int sourcePitch = image.bytesPerLine();

    for (int y = 0; y < bm.image().height(); ++y)
    {
        for (int x = 0; x < bm.image().width(); ++x)
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
        pixels += sourcePitch;
    }
    return bm;
}

Bitmap Bitmap::scaleBilinear(int sizeX, int sizeY, Palette &palette)
{
    const QRgb *rgb = palette.colorTable().constData();

    int width = subtitleImage.width();
    int height = subtitleImage.height();

    double scaleX = (double)(width - 1) / (sizeX - 1);
    double scaleY = (double)(height - 1) / (sizeY - 1);

    int lastR = 0;
    int lastG = 0;
    int lastB = 0;
    int lastA = 0;
    int lastColIdx = palette.transparentIndex();

    Bitmap trg(sizeX, sizeY);
    int prevYSI = 0;
    const uchar *inPixels = subtitleImage.constScanLine(prevYSI); // get initial scanline
    int sourcePitch = subtitleImage.bytesPerLine();

    uchar *outPixels = trg.image().scanLine(0);
    int targetPitch = trg.image().bytesPerLine();

    for (int yt = 0; yt < sizeY; ++yt)
    {
        double ys = yt * scaleY;     // source coordinate
        int ysi = (int)ys;
        double wy = (ys - ysi);
        double wy1 = 1.0 - wy;
        inPixels += ((ysi - prevYSI) * sourcePitch);

        for (int xt = 0; xt < sizeX; ++xt)
        {
            double xs = xt * scaleX;     // source coordinate
            int xsi = (int)xs;
            double wx = (xs - xsi);	// weight factor

            double wx1 = 1.0 - wx;

            // top left
            double w = wx1 * wy1;
            int idx = inPixels[xsi] & 0xff;
            double at = qAlpha(rgb[idx]) * w;
            double rt = qRed(rgb[idx]) * w;
            double gt = qGreen(rgb[idx]) * w;
            double bt = qBlue(rgb[idx]) * w;

            // top right
            if (xsi < width - 1)
            {
                w = wx * wy1;
                idx = inPixels[xsi + 1] & 0xff;
                at += qAlpha(rgb[idx]) * w;
                rt += qRed(rgb[idx]) * w;
                gt += qGreen(rgb[idx]) * w;
                bt += qBlue(rgb[idx]) * w;
            }  // else assume transparent black

            // bottom left
            if (ysi < height - 1)
            {
                w = wx1 * wy;
                idx = (inPixels + sourcePitch)[xsi] & 0xff;
                at += qAlpha(rgb[idx]) * w;
                rt += qRed(rgb[idx]) * w;
                gt += qGreen(rgb[idx]) * w;
                bt += qBlue(rgb[idx]) * w;
            } // else assume transparent black

            // bottom right
            if ((ysi < height - 1) && (xsi < width - 1))
            {
                w = wx * wy;
                idx = (inPixels + sourcePitch)[xsi + 1] & 0xff;
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
                for (idx = 0; idx < palette.size(); ++idx)
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
            outPixels[xt] = (uchar)colIdx;
        }
        prevYSI = ysi;
        outPixels += targetPitch;
    }
    return trg;
}

PaletteBitmap Bitmap::scaleBilinear(int sizeX, int sizeY, Palette &palette, bool dither)
{
    const QRgb *rgb = palette.colorTable().constData();

    int width = subtitleImage.width();
    int height = subtitleImage.height();

    double scaleX = (double)(width - 1) / (sizeX - 1);
    double scaleY = (double)(height - 1) / (sizeY - 1);

    QImage trg(sizeX, sizeY, QImage::Format_ARGB32);
    int prevYSI = 0;
    const uchar *firstScanLine = subtitleImage.constScanLine(0);
    const uchar *inPixels = subtitleImage.constScanLine(prevYSI);
    int sourcePitch = subtitleImage.bytesPerLine();

    QRgb* outPixels = (QRgb*) trg.scanLine(0);
    int targetPitch = trg.bytesPerLine() / 4; // we want pitch in QRgb values per line

    for (int yt = 0; yt < sizeY; ++yt)
    {
        double ys = yt * scaleY;     // source coordinate
        int ysi = (int)ys;
        double wy = (ys - ysi);
        double wy1 = 1.0 - wy;
        inPixels += ((ysi - prevYSI) * sourcePitch);

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
            idx = inPixels[xsi] & 0xff;
            double at = qAlpha(rgb[idx]) * w;
            double rt = qRed(rgb[idx]) * w;
            double gt = qGreen(rgb[idx]) * w;
            double bt = qBlue(rgb[idx]) * w;

            // top right
            x = xsi + 1;
            if (x < width)
            {
                w = wx * wy1;
                idx = inPixels[x] & 0xff;
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
                idx = (firstScanLine + (y * sourcePitch))[xsi] & 0xff;
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
                idx = (firstScanLine + (y * sourcePitch))[x] & 0xff;
                at += qAlpha(rgb[idx]) * w;
                rt += qRed(rgb[idx]) * w;
                gt += qGreen(rgb[idx]) * w;
                bt += qBlue(rgb[idx]) * w;
            } // else assume transparent black

            int ati = (int)(at);
            int rti = (int)(rt);
            int gti = (int)(gt);
            int bti = (int)(bt);

            outPixels[xt] = qRgba(rti, gti, bti, ati);;
        }
        prevYSI = ysi;
        outPixels += targetPitch;
    }
    // quantize image
    QuantizeFilter qf;
    Bitmap bm(sizeX, sizeY, QImage::Format_Indexed8);
    QVector<QRgb> ct = qf.quantize(trg, &bm.image(), sizeX, sizeY, 255, dither, dither);
    int size = ct.size();
    if (size > 255)
    {
        size = 255;
    }
    // create palette
    Palette trgPal(256);
    for (int i = 0; i < size; ++i)
    {
        trgPal.setARGB(i, ct[i]);
    }
    PaletteBitmap bitmap(bm, trgPal);

    return bitmap;
}

QImage Bitmap::image(Palette &palette)
{
    QImage newImage(subtitleImage.bits(), subtitleImage.width(),
                    subtitleImage.height(), subtitleImage.format());
    newImage.setColorTable(palette.colorTable());
    return newImage;
}

Bitmap Bitmap::scaleBilinearLm(int sizeX, int sizeY, Palette &palette, int alphaThreshold, QVector<int> &lumaThreshold)
{
    const uchar *cy = palette.Y().constData();
    const QRgb *a = palette.colorTable().constData();
    int width = subtitleImage.width();
    int height = subtitleImage.height();

    double scaleX = (double)(width - 1) / (sizeX - 1);
    double scaleY = (double)(height - 1) / (sizeY - 1);

    int lastCY = 0;
    int lastA  = 0;
    int lastColIdx = 0; // 0 is the transparent color

    Bitmap trg(sizeX, sizeY);
    int prevYSI = 0;
    const uchar *inPixels = subtitleImage.constScanLine(prevYSI); // get initial scanline
    int scanWidth = subtitleImage.bytesPerLine();

    uchar *outPixels = trg.image().scanLine(0); // get initial scanLine
    int targetPitch = trg.image().bytesPerLine();

    for (int yt = 0; yt < sizeY; ++yt)
    {
        double ys = yt * scaleY;     // source coordinate
        int ysi = (int)ys;
        double wy = (ys - ysi);
        double wy1 = 1.0 - wy;
        inPixels += ((ysi - prevYSI) * scanWidth);

        for (int xt = 0; xt < sizeX; ++xt)
        {
            double xs = xt * scaleX;     // source coordinate
            int xsi = (int)xs;
            double wx = (xs - xsi);	// weight factor

            double wx1 = 1.0 - wx;

            // interpolate pixel

            // top left
            double w = wx1 * wy1;
            int idx = inPixels[xsi] & 0xff;
            double at = qAlpha(a[idx]) * w;
            double cyt = (cy[idx] & 0xff) * w;

            // top right
            if (xsi < width - 1)
            {
                w = wx * wy1;
                idx = inPixels[xsi + 1] & 0xff;
                at += qAlpha(a[idx]) * w;
                cyt += (cy[idx] & 0xff) * w;
            }  // else assume transparent black

            // bottom left
            if (ysi < height - 1)
            {
                w = wx1 * wy;
                idx = (inPixels + scanWidth)[xsi] & 0xff;
                at += qAlpha(a[idx]) * w;
                cyt += (cy[idx] & 0xff) * w;
            } // else assume transparent black

            // bottom right
            if ((ysi < height - 1) && (xsi < width - 1))
            {
                w = wx * wy;
                idx = (inPixels + scanWidth)[xsi + 1] & 0xff;
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
            outPixels[xt] = (uchar)colIdx;
        }
        prevYSI = ysi;
        outPixels += targetPitch;
    }
    return trg;
}

void Bitmap::fillRect(int x1, int y1, int width, int height, int color)
{
    uchar b = (uchar)color;

    uchar* pixels = subtitleImage.scanLine(y1);
    int sourcePitch = subtitleImage.bytesPerLine();

    for (int i = y1; i < (y1 + height); ++i)
    {
        for (int j = x1; j < (x1 + width); ++j)
        {
            pixels[j] = b;
        }
        pixels += sourcePitch;
    }
}

void Bitmap::setImg(QImage &newImage)
{
    subtitleImage = newImage;
}

int Bitmap::highestColorIndex(Palette &palette)
{
    // create histogram for palette
    int maxIdx = 0;

    int width = subtitleImage.width();
    int height = subtitleImage.height();

    uchar* pixels = subtitleImage.scanLine(0);
    int sourcePitch = subtitleImage.bytesPerLine();

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            int idx = pixels[x] & 0xff;
            if (palette.alpha(idx) > 0)
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
        pixels += sourcePitch;
    }
    breakOut:
    return maxIdx;
}

QImage Bitmap::toARGB(Palette &palette)
{
    int width = subtitleImage.width();
    int height = subtitleImage.height();

    QImage newImage(width, height, QImage::Format_ARGB32);

    uchar* pixels = subtitleImage.scanLine(0);
    int sourcePitch = subtitleImage.bytesPerLine();

    QRgb* newPixels = (QRgb*)newImage.scanLine(0);
    int destPitch = newImage.bytesPerLine() / 4;

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            newPixels[x] = palette.rgba(pixels[x]);
        }
        pixels += sourcePitch;
        newPixels += destPitch;
    }
    return newImage;
}
