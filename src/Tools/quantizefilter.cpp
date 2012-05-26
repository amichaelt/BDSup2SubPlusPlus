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

#include "quantizefilter.h"
#include <algorithm>
#include <QHash>
#include <QList>

QuantizeFilter::QuantizeFilter()
{
}

void QuantizeFilter::setNumColors(int numColors)
{
    this->numColors = std::min(std::max(numColors, 8), 256);
}

QVector<int> QuantizeFilter::quantize(QRgb *inPixels, uchar *outPixels, int width, int height,
                                      int numColors, bool dither, bool serpentine)
{
    int count = width * height;
    OctTreeQuantizer quantizer;
    quantizer.setup(numColors);
    quantizer.addPixels(inPixels, 0, count);
    QVector<QRgb> table = quantizer.buildColorTable();

    if (dither)
    {
        int index = 0;
        for (int y = 0; y < height; ++y)
        {
            bool reverse = serpentine && (y & 1) == 1;
            int direction;
            if (reverse)
            {
                index = ((y * width) + width) - 1;
                direction = -1;
            }
            else
            {
                index = y * width;
                direction = 1;
            }
            for (int x = 0; x < width; ++x)
            {
                QRgb rgb1 = inPixels[index];
                int idx = quantizer.getIndexForColor(rgb1);
                QRgb rgb2 = table[idx];

                outPixels[index] = (uchar)(idx & 0xff);

                int a1 = (rgb1 >> 24) & 0xff;
                int r1 = (rgb1 >> 16) & 0xff;
                int g1 = (rgb1 >> 8) & 0xff;
                int b1 = rgb1 & 0xff;

                int a2 = (rgb2 >> 24) & 0xff;
                int r2 = (rgb2 >> 16) & 0xff;
                int g2 = (rgb2 >> 8) & 0xff;
                int b2 = rgb2 & 0xff;

                int ea = a1 - a2;
                int er = r1 - r2;
                int eg = g1 - g2;
                int eb = b1 - b2;

                for (int i = -1; i <= 1; ++i)
                {
                    int iy = i + y;
                    if (0 <= iy && iy < height)
                    {
                        for (int j = -1; j <= 1; ++j)
                        {
                            int jx = j + x;
                            if (0 <= jx && jx < width)
                            {
                                int w;
                                if (reverse)
                                {
                                    w = matrix[(((i + 1) * 3) - j) + 1];
                                }
                                else
                                {
                                    w = matrix[(((i + 1) * 3) + j) + 1];
                                }
                                if (w != 0)
                                {
                                    int k = reverse ? (index - j) : (index + j);
                                    rgb1 = inPixels[k];

                                    a1 = (rgb1 >> 24) & 0xff;
                                    r1 = (rgb1 >> 16) & 0xff;
                                    g1 = (rgb1 >> 8) & 0xff;
                                    b1 = rgb1 & 0xff;
                                    a1 += ea * w/sum;
                                    r1 += er * w/sum;
                                    g1 += eg * w/sum;
                                    b1 += eb * w/sum;
                                    inPixels[k] = (clamp(a1) << 24 | clamp(r1) << 16) | (clamp(g1) << 8) | clamp(b1);
                                }
                            }
                        }
                    }
                }
                index += direction;
            }
        }
    }

    // create palette
    QHash<int, int> p;

    for (int i = 0; i < count; ++i)
    {
        int color;
        if (dither)
        {
            color = table[outPixels[i] & 0xff];
        }
        else
        {
            color = table[quantizer.getIndexForColor(inPixels[i])];
        }
        int idx = p.size();
        int idxEx = p.value(color);
        if (idxEx == 0)
        {
            p.insert(color, idx);
        }
        else
        {
            idx = idxEx;
        }
        outPixels[i] = (uchar)(idx);
    }

    return p.keys().toVector();
}

int QuantizeFilter::clamp(int c)
{
    if (c < 0)
    {
        return 0;
    }
    if (c > 255)
    {
        return 255;
    }
    return c;
}
