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

#ifndef PALETTE_H
#define PALETTE_H

#include <QtGlobal>
#include <QVector>
#include <QColor>

class Palette
{
public:
    Palette();
    Palette(const Palette& other);
    Palette(const Palette* other);
    Palette(int paletteSize) : Palette(paletteSize, false) { }
    Palette(int paletteSize, bool use601);
    Palette(QVector<uchar> r, QVector<uchar> g, QVector<uchar> b, QVector<uchar> a, bool use601);

    void setAlpha(int index, int alpha);
    void setRGB(int index, QRgb rgb);
    QRgb getRGB(int index) { return rgba[index]; }
    void setARGB(int index, QRgb inColor);
    QRgb getARGB(int index) { return rgba[index]; }
    int getSize() { return size; }
    QColor getColor(int index) { return QColor(qRed(rgba[index]), qGreen(rgba[index]),
                                               qBlue(rgba[index]), qAlpha(rgba[index])); }
    void setColor(int index, QColor color) { setRGB(index, color.rgb()); setAlpha(index, color.alpha()); }
    int getAlpha(int index) { return qAlpha(rgba[index]); }
    QVector<uchar> getY() { return y; }
    QVector<uchar> getCb() { return cb; }
    QVector<uchar> getCr() { return cr; }
    QVector<QRgb> getColorTable() { return rgba; }
    int getTransparentIndex();
    void setYCbCr(int index, int yn, int cbn, int crn);
    QVector<int> getYCbCr(int index);

    static QVector<int> RGB2YCbCr(QRgb rgb, bool use601);

private:
    int size = 0;
    QVector<QRgb> rgba;
    QVector<uchar> y;
    QVector<uchar> cb;
    QVector<uchar> cr;
    bool useBT601 = false;

    QRgb YCbCr2RGB(int y, int cb, int cr, bool useBT601);
};

#endif // PALETTE_H
