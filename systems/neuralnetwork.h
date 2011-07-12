#ifndef __NEURAL_NETWORK__
#define __NEURAL_NETWORK__

#include "system.h"
#include "neurallayer.h"
#include "../misc/misc_package.h"

#include <vector>
#include <algorithm>
#include <math.h>
#include <assert.h>


class NeuralNetwork : public System
{
 public:
    
    friend void byte_write<>(std::ostream &out, NeuralNetwork const &network);
    friend NeuralNetwork byte_read<NeuralNetwork>(std::istream &in);
    
    typedef NeuralLayer::value_vector value_vector;
    typedef std::vector<NeuralLayer> layer_vector;
    typedef NeuralLayer::weight_type weight_type;
    
    float d_mean_weight_change;
    bool d_use_relative_learning_rate;

    value_vector d_diff;

 public:   
    std::vector<uint32_t> d_layer_sizes;
    layer_vector d_layers;

 public:
    NeuralNetwork()
    : d_mean_weight_change(0.0)
    {}
    
    NeuralNetwork(std::vector<uint32_t> layer_sizes, bool last_layer_linear = false)
      : d_mean_weight_change(0.0),
      d_use_relative_learning_rate(false)
    {
        setup(layer_sizes, last_layer_linear);
    }
    
    virtual void setup(std::vector<uint32_t> layer_sizes, bool last_layer_linear = false, NeuralLayer::InitTypes init_type = NeuralLayer::RANDOM);
    
    void input(value_vector const &some_input);
    
    value_vector &input()
    {
        return d_layers[0].input();
    }
    
    void use_relative_learning_rate(bool use_it)
    {
      d_use_relative_learning_rate = use_it;
    }
    
    virtual void print_info()
    {
        std::cerr << "nn has " << d_layers.size() << " layers" << std::endl;
        for (size_t i(0); i < d_layers.size(); ++i)
        {
            std::cerr << "layer: " << i << " ";
            d_layers[i].print_info();
        }
    }
    
    virtual void print_l1_adjust_angles()
    {
        for (size_t i(0); i < d_layers.size(); ++i)
            std::cerr << "layer " << i << " l1a-angle: " << d_layers[i].l1_adjust_mean_angle() << std::endl;
    }
    
    virtual void activate()
    {
        activate(0, d_layers.size() - 1);
    }
  
    virtual void activate(uint32_t from, uint32_t onto = 9999)
    {
        if (onto > d_layers.size())
            onto = d_layers.size() - 1;
        if (!from)
            d_layers[0].activate();
        
        for (uint32_t i(max(1, from)); i <= onto; ++i)
        {
            d_layers[i - 1].copy_activation_to(d_layers[i]);
            d_layers[i].activate();
        }
    }
    
  value_vector const &activate(value_vector const &input_values)
  {
      input(input_values);
   
      activate();
      return output();
  }
  
  value_vector &output()
  {
    return last(d_layers).output();
  }
  
  void back_propagate(value_vector const &diff, float learn_factor, bool relative_learning_rate = true)
  {
    assert(diff.size() == last(d_layer_sizes));
    
    last(d_layers).back_propagate(diff, learn_factor, relative_learning_rate);
    
    d_mean_weight_change = 0.0;
    
    for (int i(d_layers.size() - 2); i >= 0; --i)
    {
        log("back propping sub layers", TRIVIAL);
        d_layers[i].back_propagate_layer(d_layers[i + 1], learn_factor, d_use_relative_learning_rate);
    }
  }
  
  void back_propagate_middle(value_vector const &diff, float learn_factor, bool relative_learning_rate = true);
  
  void converge(float attractor_value, float learn_factor, bool use_relative_learning_rate = true)
  {
      value_vector attractor(1);
      attractor[0] = attractor_value;
      converge(attractor, learn_factor, use_relative_learning_rate);
  }
  
  
  void converge(value_vector const &attractor, float learn_factor, bool use_relative_learning_rate = true)
  {
      d_diff = vector_difference(output(), attractor);
      back_propagate(d_diff, learn_factor, use_relative_learning_rate);
  }
  
  void learn_only(uint32_t learn_layer_n)
  {
    assert(learn_layer_n >= 0 && learn_layer_n < d_layers.size());
    
    for (uint32_t i(0); i < d_layers.size(); ++i)
        d_layers[i].lock_weights(true);
    d_layers[learn_layer_n].lock_weights(false);
  }
  
  void learn_all()
  {
    for (uint32_t i(0); i < d_layers.size(); ++i)
        d_layers[i].lock_weights(false);
  }
  
  void learn_middle()
  {
    for (uint32_t i(0); i < d_layers.size(); ++i)
        d_layers[i].lock_weights(true);
    int middle = d_layers.size() / 2;
    d_layers[middle - 1].lock_weights(false);
    d_layers[middle].lock_weights(false);
  }
  
  void back_propagate(weight_type diff, float learn_factor, bool use_relative_learning_rate = true)
  {
    value_vector vec;
    vec.push_back(diff);
    back_propagate(vec, learn_factor, use_relative_learning_rate);
  }
  
  std::vector<float> const &diff() const {
      return d_diff;
  }
  
  void add_in_middle(uint32_t layer_size, float l1_norm = 0.0, NeuralLayer::InitTypes init_type = NeuralLayer::RANDOM)
  {
    int middle = d_layers.size() / 2;
    NeuralLayer input(d_layers[middle - 1].output().size(), layer_size, false, l1_norm);
    NeuralLayer output(layer_size, d_layers[middle].input().size());
    input.init(init_type);
    output.init(init_type);
    
    std::vector<NeuralLayer> layers;
    layers.push_back(input);
    layers.push_back(output);
    
    int previous_middle_size = d_layer_sizes[middle];
    d_layer_sizes.insert(d_layer_sizes.begin() + middle, layer_size);
    d_layer_sizes.insert(d_layer_sizes.begin() + middle, previous_middle_size);
    d_layers.insert(d_layers.begin() + middle, layers.begin(), layers.end());
  }

  
  void init(NeuralLayer::InitTypes init_type = NeuralLayer::RANDOM)
  {
    log("init random neural network", TRIVIAL);
    for (uint32_t i(0); i < d_layers.size(); ++i)
        d_layers[i].init(init_type);
  }
  
  layer_vector &layers()
  {
    return d_layers;
  }
  
  void save(std::string filename);
  void load(std::string filename);
  
  float mean_weight_change() {return d_mean_weight_change;}
  
  private:
};

template <>
inline void byte_write(std::ostream &out, NeuralNetwork const &network)
{
    byte_write_vector<uint32_t>(out, network.d_layer_sizes);
    byte_write_vector(out, network.d_layers);
}

template <>
inline NeuralNetwork byte_read<NeuralNetwork>(std::istream &in)
{
    std::vector<uint32_t> layer_sizes = byte_read_vector<uint32_t>(in);
    NeuralNetwork network(layer_sizes);
    
    network.d_layers = byte_read_vector<NeuralLayer>(in);
    return network;
}

#endif
