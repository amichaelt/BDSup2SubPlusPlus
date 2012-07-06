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

#include "palette.h"
#include <QColor>

Palette::Palette()
{
}

Palette::Palette(const Palette &other) :
    size(other.size),
    useBT601(other.useBT601),
    rgba(other.rgba),
    y(other.y),
    cb(other.cb),
    cr(other.cr)
{
}

Palette::Palette(const Palette *other) :
    size(other->size),
    useBT601(other->useBT601),
    rgba(other->rgba),
    y(other->y),
    cb(other->cb),
    cr(other->cr)
{
}

Palette::Palette(int paletteSize, bool use601) :
    size(paletteSize),
    useBT601(use601),
    rgba(paletteSize, 0)
{
    QVector<int> yCbCr;
    for (int i = 0; i < paletteSize; ++i)
    {
        yCbCr = Palette::RGB2YCbCr(qRgb(0, 0, 0), useBT601);
        y.push_back(yCbCr[0]);
        cb.push_back(yCbCr[1]);
        cr.push_back(yCbCr[2]);
    }
}

Palette::Palette(QVector<uchar> inRed, QVector<uchar> inGreen, QVector<uchar> inBlue, QVector<uchar> inAlpha, bool use601) :
    useBT601(use601)
{
    for (int i = 0; i < inRed.size(); ++i)
    {
        rgba.push_back(qRgba(inRed.at(i), inGreen.at(i), inBlue.at(i), inAlpha.at(i)));
    }

    QVector<int> yCbCr;
    for (int i = 0; i < rgba.size(); ++i)
    {
        yCbCr = RGB2YCbCr(rgba.at(i), useBT601);
        y.push_back(yCbCr[0]);
        cb.push_back(yCbCr[1]);
        cr.push_back(yCbCr[2]);
    }
    size = rgba.size();
}

Palette::~Palette()
{
}

void Palette::setAlpha(int index, int alpha)
{
    rgba.replace(index, qRgba(qRed(rgba.at(index)), qGreen(rgba.at(index)), qBlue(rgba.at(index)), alpha));
}

void Palette::setRGB(int index, QRgb rgb)
{
    rgba.replace(index, qRgba(qRed(rgb), qGreen(rgb), qBlue(rgb), qAlpha(rgba.at(index))));
    QVector<int> yCbCr = RGB2YCbCr(rgb, useBT601);
    y.replace(index, yCbCr[0]);
    cb.replace(index, yCbCr[1]);
    cr.replace(index, yCbCr[2]);
}

QVector<int> Palette::RGB2YCbCr(QRgb rgb, bool use601)
{
    QVector<int> yCbCr;
    double y, cb, cr;
    int r = qRed(rgb);
    int g = qGreen(rgb);
    int b = qBlue(rgb);

    if (use601)
    {
        /* BT.601 for RGB 0..255 (PC) -> YCbCr 16..235 */
        y = (((r * 0.299) * 219) / 255) + (((g * 0.587) * 219) / 255) + (((b * 0.114) * 219) / 255);
        cb = (((-r * 0.168736) * 224) / 255) - (((g * 0.331264) * 224) / 255) + (((b * 0.5) * 224) / 255);
        cr = (((r * 0.5) * 224) / 255) - (((g * 0.418688) * 224) / 255) - (((b * 0.081312) * 224) / 255);
    }
    else
    {
        /* BT.709 for RGB 0..255 (PC) -> YCbCr 16..235 */
        y = (((r * 0.2126) * 219) / 255) + (((g * 0.7152) * 219) / 255) + (((b * 0.0722) * 219) / 255);
        cb = ((((-r * 0.2126) / 1.8556) * 224) / 255) - ((((g * 0.7152) / 1.8556) * 224) / 255) + (((b * 0.5) * 224) / 255);
        cr =  (((r * 0.5) * 224) / 255) - ((((g * 0.7152) / 1.5748) * 224) / 255) - ((((b * 0.0722) / 1.5748) * 224) / 255);
    }
    yCbCr.push_back(16 + (int)(y + .5));
    yCbCr.push_back(128 + (int)(cb + .5));
    yCbCr.push_back(128 + (int)(cr + .5));

    for (int i = 0; i < yCbCr.size(); ++i)
    {
        if (yCbCr[i] < 16)
        {
            yCbCr.replace(i, 16);
        }
        else
        {
            if (i == 0)
            {
                if (yCbCr[i] > 235)
                {
                    yCbCr.replace(i, 235);
                }
            }
            else
            {
                if (yCbCr[i] > 240)
                {
                    yCbCr.replace(i, 240);
                }
            }
        }
    }

    return yCbCr;
}

QRgb Palette::YCbCr2RGB(int y, int cb, int cr, bool useBT601)
{
    double y1, r, g, b;

    y  -= 16;
    cb -= 128;
    cr -= 128;

    y1 = y * 1.164383562;
    if (useBT601)
    {
        /* BT.601 for YCbCr 16..235 -> RGB 0..255 (PC) */
        r  = y1 + (cr * 1.596026317);
        g  = y1 - (cr * 0.8129674985) - (cb * 0.3917615979);
        b  = y1 + (cb * 2.017232218);
    }
    else
    {
        /* BT.709 for YCbCr 16..235 -> RGB 0..255 (PC) */
        r  = y1 + (cr * 1.792741071);
        g  = y1 - (cr * 0.5329093286) - (cb * 0.2132486143);
        b  = y1 + (cb * 2.112401786);
    }

    r = (int)(r + 0.5);
    r = r < 0 ? 0 : r;
    r = r > 255 ? 255 : r;

    g = (int)(g + 0.5);
    g = g < 0 ? 0 : g;
    g = g > 255 ? 255 : g;

    b = (int)(b + 0.5);
    b = b < 0 ? 0 : b;
    b = b > 255 ? 255 : b;

    return qRgb(r, g, b);
}

void Palette::setARGB(int index, QRgb inColor)
{
    setRGB(index, inColor);
    setAlpha(index, qAlpha(inColor));
}

int Palette::getTransparentIndex()
{
    int transparentIndex = 0;
    int minimumAlpha = 0x100;
    for (int i = 0; i < size; ++i)
    {
        if (qAlpha(rgba.at(i)) < minimumAlpha)
        {
            minimumAlpha = qAlpha(rgba.at(i));
            transparentIndex = i;
            if (minimumAlpha == 0)
            {
                break;
            }
        }
    }
    return transparentIndex;
}

void Palette::setYCbCr(int index, int yn, int cbn, int crn)
{
    y.replace(index, (uchar)yn);
    cb.replace(index, (uchar)cbn);
    cr.replace(index, (uchar)crn);

    QRgb rgb = YCbCr2RGB(yn, cbn, crn, useBT601);

    rgba.replace(index, qRgba(qRed(rgb), qGreen(rgb), qBlue(rgb), qAlpha(rgba.at(index))));
}

QVector<int> Palette::getYCbCr(int index)
{
    QVector<int> yCbCr;
    yCbCr.push_back(y[index] & 0xff);
    yCbCr.push_back(cb[index] & 0xff);
    yCbCr.push_back(cr[index] & 0xff);
    return yCbCr;
}
