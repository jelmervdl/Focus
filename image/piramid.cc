/*
 
#include "piramid.h"
#include <math.h>
#include "../datastructs/datastructs_package.h"

Piramid::Piramid(DataElement &dataelement, float scale, uint32_t n_layers)
{
    assert(scale > 0.0 && scale < 1.0);
    
    for (uint32_t i(0); i < n_layers; ++i)
    {
        float new_scale = pow(scale, i);
     
        PixelResource resource(dataelement, new_scale);
        d_resources.push_back(resource);
        d_scales.push_back(new_scale);
        d_detail_sizes.push_back(resource.detail_size());
    }
}

float Piramid::get_pixel(float x, float y, float scale)
{
    for (uint32_t i(0); i < d_scales.size(); ++i)
        if (d_scales[i] <= scale)
            return d_resources[i].get_pixel(x, y);
    return (d_resources.end() - 1)->get_pixel(x, y);
}

*/