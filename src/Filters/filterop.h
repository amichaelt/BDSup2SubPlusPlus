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

#ifndef FILTEROP_H
#define FILTEROP_H

#include <QVector>
#include <QColor>

class Filter;
class Bitmap;
class Palette;
class QImage;

class FilterOp
{
public:
    FilterOp();
    void setFilter(Filter* value) { internalFilter = value; }

    Filter* getFilter() { return internalFilter; }

    QVector<QRgb> filter(Bitmap* src, Palette* palette, int w, int h);

    class SubSamplingData {
    public:
        SubSamplingData(QVector<int> s, QVector<int> p, QVector<float> w, int width) :
            numberOfSamples(s),
            pixelPositions(p),
            weights(w),
            matrixWidth(width)
        { }
        int matrixWidth;
        QVector<int> numberOfSamples;
        QVector<int> pixelPositions;
        QVector<float> weights;
    };

private:
    int srcWidth = 0;
    int srcHeight = 0;
    int dstWidth = 0;
    int dstHeight = 0;

    Filter* internalFilter = 0;

    SubSamplingData* horizontalSubsamplingData;
    SubSamplingData* verticalSubsamplingData;

    QVector<QRgb> rgba;

    void filterVertically(QVector<QRgb>& src, QVector<QRgb>& trg);
    void filterHorizontally(QImage* src, QVector<QRgb>& trg);

    SubSamplingData* createSubSampling(int srcSize, int dstSize, float scale);
};

#endif // FILTEROP_H
