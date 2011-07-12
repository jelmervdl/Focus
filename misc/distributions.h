#ifndef _DISTRIBUTIONS_H_
#define _DISTRIBUTIONS_H_

//#include <boost/math/distributions/normal.hpp>
//#include <boost/random/mersenne_twister.hpp>
#include <boost/random.hpp>

using namespace boost;
using namespace boost::random;
//using namespace boost::math;

inline float get_random_normal(float mean, float sigma)
{
    static mt19937 rng(static_cast<unsigned> (time(0)));
    normal_distribution<float> norm_dist(mean, sigma);
 
    variate_generator<mt19937&, normal_distribution<float> >  normal_sampler(rng, norm_dist);
 
    return normal_sampler();
}

inline float get_random_uniform(float mean, float sigma)
{
    float min = mean - sigma;
    float max = mean + sigma;
    
    return (((float)(rand() % 10000)) / 10000.) * (max - min) + min;
}

#endif
