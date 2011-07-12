#include "pixelresource.h"

PixelResource::PixelResource(DataElement &element, float scale)
{
    uint32_t image_width = element.get_meta<uint32_t>("width");
    uint32_t image_height = element.get_meta<uint32_t>("height");
    

    std::vector<float> image_pixels(element.data);
    
    if (scale == 1.0)
    {
        d_pixels = image_pixels;
        d_width = image_width;
        d_height = image_height;
        d_detail_size = max(1.0 / image_width, 1.0 / image_height);
        return;
    }
    
    uint32_t new_width = scale * image_width;
    uint32_t new_height = scale * image_height;

    d_detail_size = max(1.0 / new_width, 1.0 / new_height);    
    d_pixels.resize(new_width * new_height);
    d_width = new_width;
    d_height = new_height;
    
    for (uint32_t x(0); x < d_width; ++x)
        for (uint32_t y(0); y < d_height; ++y)
        {
            float from_x_portion = ((float) x) / d_width;
            float to_x_portion = ((float) (x + 1)) / d_width;
            float from_y_portion = ((float) y) / d_height;
            float to_y_portion = ((float) (y + 1)) / d_height;
            
            uint32_t from_x = from_x_portion * image_width;
            uint32_t to_x = to_x_portion * image_width;
            
            uint32_t from_y = from_y_portion * image_height;
            uint32_t to_y = to_y_portion * image_height;
            
            if (to_x == from_x || to_y == from_y)
            {
                d_pixels[y * d_width + x] = image_pixels[to_y * image_width + to_x];
                continue;
            }
            
            uint32_t counter(0);
            float intensity(0.0);
            for (uint32_t x_temp(from_x); x_temp < to_x; ++x_temp)
                for (uint32_t y_temp(from_y); y_temp < to_y; ++y_temp, ++counter)
                    intensity += image_pixels[y_temp * image_width + x_temp];
            intensity /= counter;
            d_pixels[y * d_width + x] = intensity;
        }
}

float &PixelResource::get_pixel(float x, float y)
{
    int x_abs(x * d_width);
    int y_abs(y * d_height);
    
    int index = y_abs * d_width + x_abs;
    
    if (index >= static_cast<int>(d_height * d_width))
        index = d_height * d_width - 1;
    if (index < 0.0)
        index = 0.0;
    return d_pixels[index];
}
