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

#include "bicubicfilter.h"
#include <cmath>

float BiCubicFilter::value(float value)
{
    if (!value)
    {
        return 1.0f;
    }
    value = std::abs(value);
    float value_squared = value * value;
    if (value < 1.0f)
    {
        return (((a_plus_2 * value_squared) * value) - (a_plus_3 * value_squared)) + 1.0f;
    }
    if (value < 2.0f)
    {
        return ((a * value_squared) * value) - (a_times_5 * value_squared) + (a_times_8 * value) - a_times_4;
    }
    return 0.0f;
}
