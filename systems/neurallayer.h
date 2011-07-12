#ifndef __NEURAL_LAYER__
#define __NEURAL_LAYER__

#include "system.h"
#include "../misc/misc_package.h"

#include <vector>
#include <algorithm>
#include <math.h>
#include <assert.h>

class NeuralLayer : public System
{
 public:
    
    friend void byte_write<>(std::ostream &out, NeuralLayer const &layer);
    friend NeuralLayer byte_read<NeuralLayer>(std::istream &in);
    
    
    typedef float weight_type;
    typedef std::vector<weight_type> value_vector;
    
    enum InitTypes {
        RANDOM,
        ZERO
    };
    
 private:   
    value_vector d_input_activation;
    value_vector d_output_activation;
    value_vector d_bias;
    value_vector d_adjustments;
    
    value_vector d_weights;
    uint32_t d_n_inputs, d_n_outputs;
    bool d_weights_are_locked;
    bool d_linear_layer;
    
  public:
    float d_l1_factor;
    Mean<float> d_l1_adjust_angle;
    
 public:
    NeuralLayer(uint32_t n_inputs, uint32_t n_outputs, bool linear_layer = false, float l1_factor = 0.0) :
        d_input_activation(n_inputs),
        d_output_activation(n_outputs),
        d_bias(n_outputs),
        d_adjustments(n_inputs),
        d_weights(n_inputs * n_outputs),
        d_n_inputs(n_inputs),
        d_n_outputs(n_outputs),
        d_weights_are_locked(false),
        d_linear_layer(linear_layer),
        d_l1_factor(l1_factor)
        {}
 
    void activate();
  
    void back_propagate(value_vector const &diff, weight_type const learn_factor, bool use_relative_learning_rate);
  
    void init(InitTypes init_type = RANDOM);

    bool lock_weights(bool lock)
    {
        return d_weights_are_locked = lock;
    }
  
  bool weights_are_locked()
  {
    return d_weights_are_locked;
  }
  
  float l1_adjust_mean_angle()
  {
    return d_l1_adjust_angle.mean(true); //true resets
  }
  
  value_vector const &activate(value_vector const &input_values)
  {
      input(input_values);
      activate();
      return output();
  }
  
  void copy_activation_to(NeuralLayer &other)
  {
    assert(d_output_activation.size() == other.d_input_activation.size());
    std::copy(d_output_activation.begin(), d_output_activation.end(), other.d_input_activation.begin());
  }
  
  virtual void input(value_vector const &some_input)
  {
      std::copy(some_input.begin(), some_input.end(), d_input_activation.begin());
  }
  
  virtual void output(value_vector const &some_output) 
  {
    std::copy(some_output.begin(), some_output.end(), d_output_activation.begin());
  }

    void print_info()
    {
          std::cerr << "size [" << d_input_activation.size() << "," << d_output_activation.size() << "] "
                << " l1: " << d_l1_factor << " locked: " << d_weights_are_locked
                << " linear: " << d_linear_layer << std::endl;
    }

  value_vector &input();
    
  value_vector &output() 
  {
    return d_output_activation;
  }
  
  
  void back_propagate_layer(NeuralLayer const &layer, weight_type const learn_factor, bool use_relative_learning_rate = false)
  {
    back_propagate(layer.d_adjustments, learn_factor, use_relative_learning_rate);
  }
  
  
  value_vector const &weights() const
  {
    return d_weights;
  }
  
  value_vector &weights()
  {
    return d_weights;
  }
  
  value_vector &biases()
  {
    return d_bias;
  }
  
 private:
    inline weight_type kernel(weight_type in)
    {
      return tanh(in);
    }
public:    
    weight_type &weight(uint32_t input_n, uint32_t output_n)
    {
        return d_weights[input_n + output_n * d_n_inputs];
    }
};

template <>
inline std::string t_to_string(NeuralLayer layer)
{
    return t_to_string<>("[") + t_to_string<>(layer.weights()) + "]";
}

template <>
inline void byte_write(std::ostream &out, NeuralLayer const &layer)
{
  byte_write<uint32_t>(out, layer.d_n_inputs);
  byte_write<uint32_t>(out, layer.d_n_outputs);
  byte_write<bool>(out, layer.d_weights_are_locked);
  byte_write<bool>(out, layer.d_linear_layer);
  
  
  byte_write_vector(out, layer.d_weights);
  byte_write_vector(out, layer.d_bias);
  byte_write_vector(out, layer.d_input_activation);
  byte_write_vector(out, layer.d_output_activation);
}

template <>
inline NeuralLayer byte_read<NeuralLayer>(std::istream &in)
{
    uint32_t n_inputs = byte_read<uint32_t>(in);
    uint32_t n_outputs = byte_read<uint32_t>(in);
    NeuralLayer layer(n_inputs, n_outputs);
    layer.d_weights_are_locked = byte_read<bool>(in);
    layer.d_linear_layer = byte_read<bool>(in);
    
    layer.d_weights = byte_read_vector<NeuralLayer::weight_type>(in);
    layer.d_bias = byte_read_vector<NeuralLayer::weight_type>(in);
    layer.d_input_activation = byte_read_vector<NeuralLayer::weight_type>(in);
    layer.d_output_activation = byte_read_vector<NeuralLayer::weight_type>(in);
    return layer;
}

#endif
