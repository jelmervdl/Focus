#ifndef __PIRAMID__
#define __PIRAMID__

#include <opencv/cv.h>

#include "pixelresource.h"

class DataElement;

class Piramid
{
    std::vector<PixelResource> d_resources;
    std::vector<float> d_scales, d_detail_sizes;
    
 public:
    
    Piramid(){}
    Piramid(DataElement &dataelement, float scale, uint32_t n_layers);
    
    float get_pixel(float x, float y, float scale = 1.0);
    
    std::vector<PixelResource> &resources() {return d_resources;}
    
};

#endif
