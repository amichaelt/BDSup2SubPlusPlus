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

#ifndef FILTEROP_H
#define FILTEROP_H

#include <vector>

class FilterOp
{
public:
    FilterOp();

private:
    class SubSamplingData {
    public:
        SubSamplingData(std::vector<int> s, std::vector<int> p, std::vector<float> w, int width) :
            numberOfSamples(s),
            pixelPositions(p),
            weights(w),
            matrixWidth(width)
        { }

    private:
        std::vector<int> numberOfSamples;
        std::vector<int> pixelPositions;
        std::vector<float> weights;
        int matrixWidth;
    };
};

#endif // FILTEROP_H
