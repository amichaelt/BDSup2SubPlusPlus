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

#include "mitchellfilter.h"

MitchellFilter::MitchellFilter() : B(1.0f / 3.0f), C(1.0f / 3.0f)
{
}

float MitchellFilter::value(float value)
{
    if (value < 0.0f)
    {
        value = -value;
    }
    float tt = value * value;
    if (value < 1.0f)
    {
        value = (((12.0f - (9.0f * B) - (6.0f * C)) * (value * tt))
                + ((-18.0f + (12.0f * B) + (6.0f * C)) * tt)
                + (6.0f - (2.0f * B)));
        return value / 6.0f;
    }
    if (value < 2.0f)
    {
        value = ((((-1.0f * B) - (6.0f * C)) * (value * tt))
                + (((6.0f * B) + (30.0f * C)) * tt)
                + (((-12.0f * B) - (48.0f * C)) * value)
                + ((8.0f * B) + (24 * C)));
        return value / 6.0f;
    }
    return 0.0f;
}
