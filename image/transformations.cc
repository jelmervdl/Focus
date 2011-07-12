#include <math.h>

#include "transformations.h"

DataElement abs_gradient(DataElement &input)
{
    std::vector<float> pixels_in(input.data), pixels_out;
    uint32_t width = input.get_meta<uint32_t>("width");
    uint32_t height = input.get_meta<uint32_t>("height");
    
    for (uint32_t y(0); y < height - 1; ++y)
        for (uint32_t x(0); x < width - 1; ++x)
        {
            float horizontal_diff = pixels_in[y * width + x + 1] - pixels_in[y * width + x];
            float vertical_diff = pixels_in[(y + 1) * width + x] - pixels_in[y * width + x];
            pixels_out.push_back(sqrt(horizontal_diff * horizontal_diff + vertical_diff * vertical_diff));
        }
    DataElement output(pixels_out);
    output.meta_data = input.meta_data;
    output.set_meta("width", width - 1);
    output.set_meta("height", height - 1);
    
    return output;
}