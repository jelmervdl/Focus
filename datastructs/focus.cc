    #include "focus.h"
    #include "../misc/misc_package.h"
    
    void FocusState::move(std::vector<float> &diff)
    {
        pos_x += diff[0];
        pos_y += diff[1];
        //radius_x += diff[2]; //hack
        //radius_y += diff[3]; //hack
        //intensity_bias += diff[4];
        //intensity_scale += diff[5];
    }
    
    
    std::string FocusState::to_string()
    {
        std::vector<float> values(2);
        values[0] = pos_x;
        values[1] = pos_y;
        
        return t_to_string(values);
    }
    
    void FocusState::from_string(std::string in)
    {
        std::vector<float> values(string_to_vector<float>(in));
        
        pos_x = values[0];
        pos_y = values[1];
    }
    
    //Focus
    
    Focus::Focus()
    : d_width(0), d_height(0)
    {}
    
    Focus::Focus(DataElement &data_element)
    {
        load_image(data_element);
    }
    
    void Focus::load_image(DataElement &dataelement)
    {
        d_pixels = dataelement.data;
        d_width = dataelement.get_meta<int>("width");
        d_height = dataelement.get_meta<int>("height");
    }
    
    
    std::vector<float> Focus::sub_image(int start_x, int start_y, int width, int height)
    {
        std::vector<float> sub_image_pixels(width * height);
        for (int i(0); i < height; ++i)
            copy(d_pixels.begin() + ((start_y + i) * d_width + start_x),
                 d_pixels.begin() + ((start_y + i) * d_width + start_x + width),
                 sub_image_pixels.begin() + i * width);
        return sub_image_pixels;
    }
    
    void Focus::set_sub_image(int start_x, int start_y, int width, int height, std::vector<float> &pixels)
    {
      assert(width * height == (int) pixels.size());///HACK
        for (int i(0); i < height; ++i)
            copy(pixels.begin() + i * width,
                 pixels.begin() + i * width + width,
                 d_pixels.begin() + ((start_y + i) * d_width + start_x));
    }
