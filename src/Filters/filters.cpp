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

#include "filter.h"
#include "filters.h"
#include "bellfilter.h"
#include "bicubicfilter.h"
#include "bsplinefilter.h"
#include "hermitefilter.h"
#include "lanczos3filter.h"
#include "mitchellfilter.h"
#include "trianglefilter.h"

Filters::Filters()
{
}

Filter *Filters::getFilter(ScalingFilters scalingFilter)
{
    switch (scalingFilter)
    {
        case ScalingFilters::BELL:
        {
            if (bellFilter == 0)
            {
                bellFilter = new BellFilter;
            }
            return bellFilter;
        } break;
        case ScalingFilters::BICUBIC:
        {
            if (biCubicFilter == 0)
            {
                biCubicFilter = new BiCubicFilter;
            }
            return biCubicFilter;
        }    break;
        case ScalingFilters::BSPLINE:
        {
            if (bSplineFilter == 0)
            {
                bSplineFilter = new BSplineFilter;
            }
            return bSplineFilter;
        }   break;
        case ScalingFilters::HERMITE:
        {
            if (hermiteFilter == 0)
            {
                hermiteFilter = new HermiteFilter;
            }
            return hermiteFilter;
        }    break;
        case ScalingFilters::LANCZOS3:
        {
            if (lanczos3Filter == 0)
            {
                lanczos3Filter = new Lanczos3Filter;
            }
            return lanczos3Filter;
        }   break;
        case ScalingFilters::MITCHELL:
        {
            if (mitchellFilter == 0)
            {
                mitchellFilter = new MitchellFilter;
            }
            return mitchellFilter;
        }   break;
        case ScalingFilters::TRIANGLE:
        {
            if (triangleFilter == 0)
            {
                triangleFilter = new TriangleFilter;
            }
            return triangleFilter;
        }   break;
        default:
        {
            return 0;
        }
    }
}
