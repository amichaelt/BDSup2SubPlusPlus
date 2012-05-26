/*
BDSup2Sub++ (C) 2012 Adam T.
Base on code Copyright 2006 Jerry Huxtable and modified for BDSup2Sub by Volker Oth/0xdeadbeef (extended to alpha component)

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

   http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#ifndef QUANTIZEFILTER_H
#define QUANTIZEFILTER_H

#include <QVector>
#include <QColor>

/**
 * Floyd-Steinberg dithering matrix.
 */
static QVector<int> matrix = {
    0, 0, 0,
    0, 0, 7,
    3, 5, 1,
};

class QuantizeFilter
{
public:
    QuantizeFilter();

    void setNumColors(int numColors);
    int getNumColors() { return numColors; }
    void setDither(bool dither) { this->dither = dither; }
    bool getDither() { return dither; }
    void setSerpentine(bool serpentine) { this->serpentine = serpentine; }
    bool getSerpentine() { return serpentine; }
    QVector<int> quantize(QRgb* inPixels, uchar* outPixels, int width, int height,
                           int numColors, bool dither, bool serpentine);
    int clamp(int c);



private:
    static constexpr int sum = 3 + 5 + 7 + 1;
    bool dither = false;
    bool serpentine = false;
    int numColors = 255;
};

#endif // QUANTIZEFILTER_H
