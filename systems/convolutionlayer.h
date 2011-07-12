#ifndef __CONVOLUTION_LAYER__
#define __CONVOLUTION_LAYER__

#include "system.h"
#include "neuralnetwork.h"
#include "../misc/misc_package.h"
#include "../datastructs/datastructs_package.h"
#include "../systems/systems_package.h"


#include <vector>
#include <algorithm>
#include <math.h>
#include <assert.h>
#include <iostream>

class ConvolutionLayer : public System
{
 public:
    friend void byte_write<>(std::ostream &out, ConvolutionLayer const &layer);
    friend ConvolutionLayer byte_read<ConvolutionLayer>(std::istream &in);
    
    
    typedef float weight_type;
    typedef std::vector<weight_type> value_vector;
    
    enum InitTypes {
        RANDOM,
        ZERO
    };
    
    AutoEncoder d_ae;
    Mean<float> d_mean;
    
 private:   
    
  public:
    
    
 public:
    ConvolutionLayer();
            
    float mean_error()
    {
        return d_mean.mean();
    }
    
    void reset_mean_error()
    {
        d_mean.reset();
    }
    
  void init(InitTypes init_type = RANDOM)
  {
  }
  
  void learn(DataElement &dataelement);  
  
  value_vector encode(DataElement &dataelement);
  
  void decode(value_vector &values, DataElement *dataelement_ptr);
  
  void set_learning_rate(float learning_rate)
  {
    d_ae.set_learning_rate(learning_rate);
  }
};

template <>
inline void byte_write(std::ostream &out, ConvolutionLayer const &layer)
{
    byte_write(out, layer.d_ae);
}

template <>
inline ConvolutionLayer byte_read<ConvolutionLayer>(std::istream &in)
{
    ConvolutionLayer layer;
    layer.d_ae = byte_read<AutoEncoder>(in);
    return layer;
}

#endif
