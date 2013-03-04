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

#ifndef BSPLINEFILTER_H
#define BSPLINEFILTER_H

#include "filter.h"

class BSplineFilter : public Filter
{
public:
    float radius() { return 2.0f; }
    float value(float value);

    static constexpr float two_thirds = 2.0f / 3.0f;
    static constexpr float one_sixth = 1.0f / 6.0f;
};

#endif // BSPLINEFILTER_H
