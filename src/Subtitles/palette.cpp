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

#include "palette.h"
#include <QColor>

Palette::Palette()
{
}

Palette::Palette(const Palette &other) :
    size(other.size),
    r(other.r),
    g(other.g),
    b(other.b),
    a(other.a),
    y(other.y),
    cb(other.cb),
    cr(other.cr),
    useBT601(other.useBT601)
{
}

Palette::Palette(const Palette *other) :
    size(other->size),
    r(other->r),
    g(other->g),
    b(other->b),
    a(other->a),
    y(other->y),
    cb(other->cb),
    cr(other->cr),
    useBT601(other->useBT601)
{
}

Palette::Palette(int paletteSize, bool use601) :
    size(paletteSize),
    r(paletteSize, 0),
    g(paletteSize, 0),
    b(paletteSize, 0),
    a(paletteSize, 0),
    useBT601(use601)
{
    QVector<int> yCbCr;
    for (int i = 0; i < paletteSize; ++i)
    {
        yCbCr = Palette::RGB2YCbCr(0, 0, 0, useBT601);
        y.push_back(yCbCr[0]);
        cb.push_back(yCbCr[1]);
        cr.push_back(yCbCr[2]);
    }
}

Palette::Palette(QVector<uchar> inRed, QVector<uchar> inGreen, QVector<uchar> inBlue, QVector<uchar> inAlpha, bool use601) :
    r(inRed),
    g(inGreen),
    b(inBlue),
    a(inAlpha),
    useBT601(use601)
{
    QVector<int> yCbCr;
    for (int i = 0; i < r.size(); ++i)
    {
        yCbCr = RGB2YCbCr(r[i], g[i], b[i], useBT601);
        y.push_back(yCbCr[0]);
        cb.push_back(yCbCr[1]);
        cr.push_back(yCbCr[2]);
    }
    size = r.size();
}

void Palette::setAlpha(int index, int alpha)
{
    a.replace(index, alpha);
}

void Palette::setRGB(int index, int red, int green, int blue)
{
    r.replace(index, red);
    g.replace(index, green);
    b.replace(index, blue);
    QVector<int> yCbCr = RGB2YCbCr(red, green, blue, useBT601);
    y.replace(index, yCbCr[0]);
    cb.replace(index, yCbCr[1]);
    cr.replace(index, yCbCr[2]);
}

QVector<int> Palette::getRGB(int index)
{
    QVector<int> rgb(3);

    rgb.replace(0, r[index] & 0xff);
    rgb.replace(1, g[index] & 0xff);
    rgb.replace(2, b[index] & 0xff);

    return rgb;
}

QVector<int> Palette::RGB2YCbCr(int r, int g, int b, bool use601)
{
    QVector<int> yCbCr;
    double y, cb, cr;

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

QVector<int> Palette::YCbCr2RGB(int y, int cb, int cr, bool useBT601)
{
    QVector<int> rgb(3);
    double y1, r, g, b;

    y  -= 16;
    cb -= 128;
    cr -= 128;

    y1 = y * 1.164383562;
    if (useBT601)
    {
        /* BT.601 for YCbCr 16..235 -> RGB 0..255 (PC) */
        r  = y1 + (cr * 1.596026317);
        g  = y1 - (cr*0.8129674985) - (cb * 0.3917615979);
        b  = y1 + (cb * 2.017232218);
    }
    else
    {
        /* BT.709 for YCbCr 16..235 -> RGB 0..255 (PC) */
        r  = y1 + (cr * 1.792741071);
        g  = y1 - (cr * 0.5329093286) - (cb * 0.2132486143);
        b  = y1 + (cb * 2.112401786);
    }

    rgb.replace(0, (int)(r + 0.5));
    rgb.replace(1, (int)(g + 0.5));
    rgb.replace(2, (int)(b + 0.5));
    for (int i = 0; i < 3; ++i)
    {
        if (rgb[i] < 0)
        {
            rgb.replace(i, 0);
        }
        else if (rgb[i] > 255)
        {
            rgb.replace(i, 255);
        }
    }
    return rgb;
}

void Palette::setARGB(int index, unsigned int inColor)
{
    setRGB(index, qRed(inColor), qGreen(inColor), qBlue(inColor));
    setAlpha(index, qAlpha(inColor));
}

int Palette::getARGB(int index)
{
    return qRgba(r[index] & 0xff, g[index] & 0xff, b[index] & 0xff, a[index] & 0xff);
}

QVector<QRgb> Palette::getColorTable()
{
    QVector<QRgb> colorTable;

    for (int i = 0; i < size; ++i)
    {
        colorTable.push_back(qRgba(r[i] & 0xff, g[i] & 0xff, b[i] & 0xff, a[i] & 0xff));
    }
    return colorTable;
}

int Palette::getTransparentIndex()
{
    int transparentIndex = 0;
    int minimumAlpha = 0x100;
    for (int i = 0; i < size; ++i)
    {
        if (a[i] < minimumAlpha)
        {
            minimumAlpha = a[i];
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

    QVector<int> rgb = YCbCr2RGB(yn, cbn, crn, useBT601);

    r.replace(index, (uchar)rgb[0]);
    g.replace(index, (uchar)rgb[1]);
    b.replace(index, (uchar)rgb[2]);
}
