#include "rl_system.h"

void RLSystem::save(std::string filename)
{
  std::ofstream out(filename.c_str());
  byte_write<RLSystem>(out, *this);
}

void RLSystem::load(std::string filename)
{
  std::ifstream in(filename.c_str());
  *this = byte_read<RLSystem>(in);
  
    d_conditions.ok("alpha", true);
    d_conditions.ok("beta", true);
    d_conditions.ok("gamma", true);
    d_conditions.ok("max_step", true);
    d_conditions.ok("v_layers", true);
    d_conditions.ok("ac_layers", true);
}

float get_state_performance(FocusState &s1, FocusState &s2, float sigma)
{
    float x_diff(s1.pos_x - s2.pos_x);
    float y_diff(s1.pos_y - s2.pos_y);
    
    float distance = x_diff * x_diff + y_diff * y_diff;
    
    //return 10. - 64. * sqrt(distance);
    float performance = exp(-(distance) / (2.0 * (sigma * sigma)));
    return performance;
}

float state_distance(FocusState &s1, FocusState &s2)
{
    float x_diff(s1.pos_x - s2.pos_x);
    float y_diff(s1.pos_y - s2.pos_y);
    
    float distance = x_diff * x_diff + y_diff * y_diff;
    return sqrt(distance);
}
