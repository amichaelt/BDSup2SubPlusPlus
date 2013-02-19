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

#ifndef MITCHELLFILTER_H
#define MITCHELLFILTER_H

#include "filter.h"

class MitchellFilter : public Filter
{
public:
    float getRadius() { return 2.0f; }
    float value(float value);

private:
    // compile time calculation of all needed values
    static constexpr float B = 1.0f / 3.0f;
    static constexpr float C = 1.0f / 3.0f;
    static constexpr float divisor = 1.0f / 6.0f;

    static constexpr float _12_minus_9B_minus_6C = 12.0f - (9.0f * B) - (6.0f * C);
    static constexpr float _minus_18_plus_12B_plus_6C = -18.0f + (12.0f * B) + (6.0f * C);
    static constexpr float _6_minus_2B = 6.0f - (2.0f * B);

    static constexpr float _minus_B_minus_6C = (-B) - (6.0f * C);
    static constexpr float _6B_plus_30C = (6.0f * B) + (30.0f * C);
    static constexpr float _minus_12B_minus_48C = (-12.0f * B) - (48.0f * C);
    static constexpr float _8B_plus_24C = (8.0f * B) + (24 * C);
};

#endif // MITCHELLFILTER_H
