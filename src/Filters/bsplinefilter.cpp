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

#include "bsplinefilter.h"
#include <cmath>

float BSplineFilter::value(float value)
{
    value = std::abs(value);
    if (value < 1.0f)
    {
        float tt = value * value;
        return 0.5f * tt * value - tt + (2.0f / 3.0f);
    }
    if (value < 2.0f)
    {
        value = 2.0f - value;
        return (1.0f / 6.0f) * value * value * value;
    }
    return 0.0f;
}
