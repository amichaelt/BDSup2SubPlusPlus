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
    Palette(QVector<QRgb> inRgba, bool use601);

    void setAlpha(int index, int alpha);
    int getAlpha(int index) { return qAlpha(rgba[index]); }
    QVector<QRgb> getAlpha() { return rgba; }

    void setRGB(int index, QRgb inRgb);
    QRgb getRGB(int index) { return rgba[index]; }

    void setARGB(int index, unsigned int inColor){ setRGB(index, inColor); setAlpha(index, qAlpha(inColor)); }
    QRgb getARGB(int index) { return rgba[index]; }

    void setColor(int index, QColor color) { setRGB(index, color.rgb()); setAlpha(index, color.alpha()); }
    QColor getColor(int index) { return QColor(qRed(rgba[index]), qGreen(rgba[index]), qBlue(rgba[index]), qAlpha(rgba[index])); }

    QVector<QRgb> getColorTable() { return rgba; }

    void setYCbCr(int index, int yn, int cbn, int crn);
    QVector<uchar> getY() { return y; }
    QVector<uchar> getCb() { return cb; }
    QVector<uchar> getCr() { return cr; }
    QVector<int> getYCbCr(int index);

    static QVector<int> RGB2YCbCr(QRgb rgb, bool use601);

    int getSize() { return size; }
    int getTransparentIndex();

private:
    int size = 0;
    QVector<QRgb> rgba;
    QVector<uchar> y = QVector<uchar>();
    QVector<uchar> cb = QVector<uchar>();
    QVector<uchar> cr = QVector<uchar>();
    bool useBT601 = false;

    QRgb YCbCr2RGB(int y, int cb, int cr, bool useBT601);
};

#endif // PALETTE_H
