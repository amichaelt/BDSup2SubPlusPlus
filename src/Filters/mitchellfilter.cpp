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

#include "mitchellfilter.h"
#include <cmath>

float MitchellFilter::value(float value)
{
    value = std::abs(value);
    float value_square = value * value;
    float value_cubed = value_square * value;

    if (value < 1.0f)
    {
        return ((_12_minus_9B_minus_6C * value_cubed)
                + (_minus_18_plus_12B_plus_6C * value_square)
                + _6_minus_2B) * divisor;
    }
    if (value < 2.0f)
    {
        return ((_minus_B_minus_6C * value_cubed)
                + (_6B_plus_30C * value_square)
                + (_minus_12B_minus_48C * value)
                + _8B_plus_24C) * divisor;
    }
    return 0.0f;
}
