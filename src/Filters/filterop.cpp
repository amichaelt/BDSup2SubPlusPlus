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

#include "filterop.h"
#include "Filters/mitchellfilter.h"
#include "Subtitles/bitmap.h"
#include "Subtitles/palette.h"
#include <algorithm>

#include "xmmintrin.h"

#include <cmath>
#include <QImage>

FilterOp::FilterOp(Filter& filter) :
    internalFilter(filter)
{
}

QVector<QRgb> FilterOp::filter(Bitmap &src, Palette &palette, int w, int h)
{
    dstWidth = w;
    dstHeight = h;
    srcWidth = src.width();
    srcHeight = src.height();

    float xscale = (float)(dstWidth - 1) / (float)(srcWidth - 1);
    float yscale = (float)(dstHeight - 1) / (float)(srcHeight - 1);

    // Precalculate  subsampling/supersampling
    horizontalSubsamplingData = createSubSampling(srcWidth, dstWidth, xscale);
    verticalSubsamplingData = createSubSampling(srcHeight, dstHeight, yscale);

    QVector<QRgb> workPixels(srcHeight * dstWidth);
    QImage inImage = src.image(palette);
    filterHorizontally(inImage, workPixels.data(), palette.colorTable().constData());

    QVector<QRgb> outPixels(dstHeight * dstWidth);
    filterVertically(workPixels, outPixels);

    return outPixels;
}

FilterOp::SubSamplingData FilterOp::createSubSampling(int srcSize, int dstSize, float scale)
{
    QVector<int> arrN(dstSize);
    int numContributors;
    QVector<float> arrWeight;
    QVector<int> arrPixel;

    float fwidth = internalFilter.getRadius();

    if (scale < 1.0f)
    {
        // scale down -> subsampling
        float width = fwidth / scale;
        numContributors = (int)((width * 2.0f) + 2); // Heinz: added 1 to be safe with the ceiling
        arrWeight = QVector<float>(dstSize * numContributors);
        arrPixel = QVector<int>(dstSize * numContributors);

        float fNormFac = (float)(1.0f / (ceil(width) / fwidth));
        //
        for (int i = 0; i < dstSize; ++i)
        {
            arrN[i] = 0;
            int subindex = i * numContributors;
            float center = i / scale;
            int left = (int)floor(center - width);
            int right = (int)ceil(center + width);
            for (int j = left; j <= right; ++j)
            {
                float weight;
                weight = internalFilter.value((center - j) * fNormFac);

                if (weight == 0.0f)
                {
                    continue;
                }
                int n;
                if (j < 0)
                {
                    n = -j;
                }
                else if (j >= srcSize)
                {
                    n = ((srcSize - j) + srcSize) - 1;
                }
                else
                {
                    n = j;
                }

                int k = arrN[i];
                arrN[i] = arrN[i] + 1;
                if (n < 0 || n >= srcSize)
                {
                    weight= 0.0f;// Flag that cell should not be used
                }

                arrPixel[subindex + k] = n;
                arrWeight[subindex + k] = weight;
            }
            // normalize the filter's weight's so the sum equals to 1.0, very important for avoiding box type of artifacts
            int max = arrN[i];
            float tot = 0;
            for (int k = 0; k < max; ++k)
            {
                tot += arrWeight[subindex + k];
            }
            if (tot != 0.0f)
            {
                // 0 should never happen except bug in filter
                for (int k = 0; k < max; ++k)
                {
                    arrWeight[subindex + k] = arrWeight[subindex + k] / tot;
                }
            }
        }
    }
    else
    {
        // scale up -> super-sampling
        numContributors = (int)((fwidth * 2.0f) + 1);
        arrWeight = QVector<float>(dstSize * numContributors);
        arrPixel = QVector<int>(dstSize * numContributors);
        //
        for (int i = 0; i < dstSize; ++i)
        {
            arrN[i] = 0;
            int subindex = i * numContributors;
            float center = i / scale;
            int left = (int)floor(center - fwidth);
            int right = (int)ceil(center + fwidth);
            for (int j = left; j <= right; ++j)
            {
                float weight = internalFilter.value(center - j);
                if (weight == 0.0f)
                {
                    continue;
                }
                int n;
                if (j < 0)
                {
                    n = -j;
                }
                else if (j >= srcSize)
                {
                    n = ((srcSize - j) + srcSize) - 1;
                }
                else
                {
                    n = j;
                }

                int k = arrN[i];
                arrN[i] = arrN[i]+ 1;
                if (n < 0 || n >= srcSize)
                {
                    weight= 0.0f;// Flag that cell should not be used
                }

                arrPixel[subindex + k] = n;
                arrWeight[subindex + k] = weight;
            }
            // normalize the filter's weights so the sum equals to 1.0, very important for avoiding box type of artifacts
            int max = arrN[i];
            float tot = 0;
            for (int k = 0; k < max; ++k)
            {
                tot += arrWeight[subindex + k];
            }
            if (tot != 0.0f)
            {
                for (int k = 0; k < max; ++k)
                {
                    arrWeight[subindex + k] = arrWeight[subindex + k] / tot;
                }
            }
        }
    }
    return SubSamplingData(arrN, arrPixel, arrWeight, numContributors);
}

void FilterOp::filterVertically(QVector<QRgb>& src, QVector<QRgb>& trg)
{
    const QRgb *inPixels = src.constData();

    for (int x = 0; x < dstWidth; ++x)
    {
        for (int y = dstHeight - 1; y >= 0 ; --y)
        {
            int yTimesNumContributors = y * verticalSubsamplingData.matrixWidth;
            int max = verticalSubsamplingData.numberOfSamples[y];
            int ofsY = dstWidth * y;
            float red = 0;
            float green = 0;
            float blue = 0;
            float alpha = 0;

            int index = yTimesNumContributors;
            for (int j = max - 1; j >= 0 ; --j)
            {
                int color = inPixels[x + (dstWidth * verticalSubsamplingData.pixelPositions[index])];
                float w = verticalSubsamplingData.weights[index];
                alpha += qAlpha(color) * w;
                red += qRed(color) * w;
                green += qGreen(color) * w;
                blue += qBlue(color) * w;
                index++;
            }

            int ri = std::max((int)red, 0);
            ri = std::min(ri, 255);

            int gi = std::max((int)green, 0);
            gi = std::min(gi, 255);

            int bi = std::max((int)blue, 0);
            bi = std::min(bi, 255);

            int ai = std::max((int)alpha, 0);
            ai = std::min(ai, 255);

            trg[x + ofsY] = qRgba(ri, gi, bi, ai);
        }
    }
}

void FilterOp::filterHorizontally(QImage &src, QRgb *trg, const QRgb *rgba)
{
    const uchar* pixels = src.constScanLine(0);
    int sourcePitch = src.bytesPerLine();

    for (int k = 0; k < srcHeight; ++k)
    {
        int destOfsY = dstWidth * k;
        for (int i = dstWidth - 1; i >= 0 ; --i)
        {
            float red = 0;
            float green = 0;
            float blue = 0;
            float alpha = 0;

            int max = horizontalSubsamplingData.numberOfSamples[i];
            int index = i * horizontalSubsamplingData.matrixWidth;

            for (int j = max - 1; j >= 0; --j)
            {
                int ofsX = horizontalSubsamplingData.pixelPositions[index];
                int palIdx = pixels[ofsX] & 0xff;
                float w = horizontalSubsamplingData.weights[index];
                red += qRed(rgba[palIdx]) * w;
                green += qGreen(rgba[palIdx]) * w;
                blue+= qBlue(rgba[palIdx]) * w;
                alpha += qAlpha(rgba[palIdx]) * w;
                index++;
            }

            int ri = std::max((int)red, 0);
            ri = std::min(ri, 255);

            int gi = std::max((int)green, 0);
            gi = std::min(gi, 255);

            int bi = std::max((int)blue, 0);
            bi = std::min(bi, 255);

            int ai = std::max((int)alpha, 0);
            ai = std::min(ai, 255);

            trg[i + destOfsY] = qRgba(ri, gi, bi, ai);
        }
        pixels += sourcePitch;
    }
}
