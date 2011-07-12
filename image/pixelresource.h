#ifndef __PIXELRESOURCE__
#define __PIXELRESOURCE__

#include <vector>
#include "../datastructs/dataelement.h"
#include "../misc/general.h"

class PixelResource
{
    std::vector<float> d_pixels;
    uint32_t d_width, d_height;
    float d_detail_size;
    
 public:
    PixelResource(DataElement &element, float reduction = 1.0);
    
    float &get_pixel(float x, float y);
    
    std::vector<float> &pixels() {return d_pixels;}
    
    float width(){return d_width;}
    float height(){return d_height;}
    
    DataElement get_dataelement()
    {
        DataElement dataelement(d_pixels);
        dataelement.set_meta("width", d_width);
        dataelement.set_meta("height", d_height);
        return dataelement;
    }
    
    float detail_size() {return d_detail_size;}
};

#endif
