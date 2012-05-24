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

#ifndef FILTERS_H
#define FILTERS_H

#include "types.h"

class Filter;
class BellFilter;
class BiCubicFilter;
class BSplineFilter;
class HermiteFilter;
class Lanczos3Filter;
class MitchellFilter;
class TriangleFilter;

static BellFilter* bellFilter = 0;
static BiCubicFilter* biCubicFilter = 0;
static BSplineFilter* bSplineFilter = 0;
static HermiteFilter* hermiteFilter = 0;
static Lanczos3Filter* lanczos3Filter = 0;
static MitchellFilter* mitchellFilter = 0;
static TriangleFilter* triangleFilter = 0;

class Filters
{
public:
    Filters();

    static Filter* getFilter(ScalingFilters scalingFilter);
};

#endif // FILTERS_H
