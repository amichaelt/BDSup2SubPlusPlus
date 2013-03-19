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

#include "lanczos3filter.h"
#include <cmath>

Lanczos3Filter::Lanczos3Filter() :
    PI_FLOAT(std::atan(1.0) * 4)
{
}

float Lanczos3Filter::value(float value)
{
    if (!value)
    {
        return 1.0f;
    }

    value = std::abs(value);
    if (value < 3.0f)
    {
        value *= PI_FLOAT;
        double value_divided_by_3 = value / 3.0f;
        return sincModified(value) * sincModified();
    }
    return 0.0f;
}

static constexpr float sincModified(float value)
{
    return std::sin(value) / value;
}
