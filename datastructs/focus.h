#ifndef __FOCUS__
#define __FOCUS__

#include "../datastructs/dataelement.h"

#include <math.h>
#include <string>

struct FocusState {
    
    FocusState(float _pos_x = 0.0, float _pos_y = 0.0, float _radius = 1.0)
        : pos_x(_pos_x), pos_y(_pos_y), radius(_radius)
        {
        }
    
    FocusState(std::string in)
    {
        from_string(in);
    }
    
    void sanity_check()
    {
        float const min_radius(0.01);
        if (radius < min_radius)
            radius = min_radius;
        
        pos_x = max(pos_x, 0.0);
        pos_x = min(pos_x, 1.0);
        pos_y = max(pos_y, 0.0);
        pos_y = min(pos_y, 1.0);
    }
    
    std::vector<float> state_vector()
    {
        std::vector<float> return_vector(2);
        
        return_vector[0] = pos_x;
        return_vector[1] = pos_y;
        
        return return_vector;
    }
    
    void move(std::vector<float> &diff);
    
    //post
    float pos_x, pos_y;
    float radius;
    
    
    std::string to_string();
    void from_string(std::string in);    
};

class Focus
{
    std::vector<float> d_pixels;
    int d_width, d_height;
  public:
    
    Focus();
    Focus(DataElement &data_element);
    
    void load_image(DataElement &dataelement);
    
    std::vector<float> focus_vector(FocusState const &focus_state)
    {
        std::vector<float> focus_vec(focus_state.radius * 2 * focus_state.radius * 2);
        int from_x = focus_state.pos_x * d_width - focus_state.radius;
        int to_x= from_x + focus_state.radius * 2;
        int from_y = focus_state.pos_y * d_height - focus_state.radius;
        int to_y = from_y + focus_state.radius * 2;
        
        int i(0);
        for (int y(from_y); y < to_y; ++y)
            for (int x(from_x); x < to_x; ++x, ++i)
                focus_vec[i] = pixel(x, y);
        return focus_vec;
    }
    
    float pixel(int x, int y)
    {
        if (x <= 0) x = 1;
        if (x >= d_width) x = d_width - 1;
        if (y <= 0) y = 1;
        if (y >= d_height) y = d_height - 1;
        
        return d_pixels[y * d_width + x];
    }
  
    std::vector<float> sub_image(int start_x, int start_y, int width, int height);
    void set_sub_image(int start_x, int start_y, int width, int height, std::vector<float> &pixels);
    
    std::vector<float> &pixels()
    {
        return d_pixels;
    }
};

#endif
