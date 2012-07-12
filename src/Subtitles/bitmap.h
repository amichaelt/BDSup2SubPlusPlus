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

#ifndef BITMAP_H
#define BITMAP_H

#include <QImage>

template <typename T> class QVector;

class Filter;
class Palette;
class PaletteBitmap;
class QRect;

class Bitmap
{
public:
    Bitmap();
    Bitmap(Bitmap &other);
    Bitmap(Bitmap *other);
    Bitmap(int width, int height);
    Bitmap(int width, int height, int color);
    Bitmap(QImage subtitleImage);

    void clear(int color);
    void fillRect(int x1, int y1, int width, int height, int color);
    void setImg(QImage &newImage);

    inline int height() { return subtitleImage.height(); }
    inline int width() { return subtitleImage.width(); }

    int highestColorIndex(Palette &palette);
    int primaryColorIndex(Palette &palette, int alphaThreshold);

    Bitmap crop(int x1, int y1, int width, int height);
    Bitmap convertLm(Palette &palette, int alphaThreshold, QVector<int> &lumaThreshold);

    Bitmap scaleFilter(int sizeX, int sizeY, Palette &palette, Filter &filter);
    Bitmap scaleFilterLm(int sizeX, int sizeY, Palette &palette,
                          int alphaThreshold, QVector<int> &lumaThreshold, Filter& filter);
    Bitmap scaleBilinear(int sizeX, int sizeY, Palette &palette);
    Bitmap scaleBilinearLm(int sizeX, int sizeY, Palette &palette,
                            int alphaThreshold, QVector<int> &lumaThreshold);

    PaletteBitmap scaleFilter(int sizeX, int sizeY, Palette &palette, Filter &filter, bool dither);
    PaletteBitmap scaleBilinear(int sizeX, int sizeY, Palette &palette, bool dither);

    QImage &image() { return subtitleImage; }
    QImage image(Palette &palette);

    QImage toARGB(Palette &palette);

    QRect bounds(Palette &palette, int alphaThreshold);

private:
    QImage subtitleImage;
};

#endif // BITMAP_H
