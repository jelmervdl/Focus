#ifndef __AUTO_ENCODER__
#define __AUTO_ENCODER__

#include "neuralnetwork.h"
#include "../misc/misc_package.h"
#include <algorithm>

class AutoEncoder : public NeuralNetwork
{
    
    friend void byte_write<>(std::ostream &out, AutoEncoder const &encoder);
    friend AutoEncoder byte_read<AutoEncoder>(std::istream &in);
    
    typedef NeuralNetwork::value_vector value_vector;
    float d_learn_rate;
  
  public:
    AutoEncoder() : d_learn_rate(0.0){}  
    AutoEncoder(NeuralNetwork const &network)
    :
    NeuralNetwork(),
    d_learn_rate(0.0)
    {
        this->d_layer_sizes = network.d_layer_sizes;
        this->d_layers = network.d_layers;
    }  
    
    void set_learning_rate(float learn_rate)
    {
        d_learn_rate = learn_rate;
    }
    
    value_vector const &activate(value_vector const &input_values)
    {
        return activate(input_values, input_values);                            
    }
    
    value_vector const &activate(value_vector const &input_values, value_vector const &original_vector)
    {
        NeuralNetwork::activate(input_values);
        d_diff = vector_difference(output(), original_vector);
        return output();
    }
    
    value_vector const &get_encoding(value_vector const &input_values)
    {
        NeuralNetwork::activate(input_values);
        //uint32_t encoding_size(*encoding_ptr);
        uint32_t index(d_layer_sizes.size() / 2);
        return d_layers[index].input();
    }
    
    value_vector const &get_encoding()
    {
        uint32_t index(d_layer_sizes.size() / 2);
        
        return d_layers[index].input();
    }

    std::string layer_string() {
        std::string return_string("[");
        for (size_t i(0); i < d_layers.size() / 2; ++i)
            return_string += t_to_string(d_layers[i].input().size()) + ",";
        return_string += t_to_string(d_layers[d_layers.size() / 2].input().size()) + "]";
        return return_string;
    }
    
    value_vector decode(value_vector const &input_values);
    
    void set_l1(float l1_value)
    {
        for (size_t i(0); i < d_layers.size(); ++i)
            d_layers[i].d_l1_factor = l1_value;
    }
    
    void set_middle_l1(float l1_value)
    {
        unsigned int middle = d_layers.size() / 2 - 1;
        
        set_l1(0.0);
        d_layers[middle].d_l1_factor = l1_value;
    }
    
    void learn(value_vector const &input)
    {
        learn(input, input);
    }
    
    void learn(value_vector const &input, value_vector const &original_vector, bool relative = true)
    {
        activate(input, original_vector);

        back_propagate(d_diff, d_learn_rate, relative);
    }
    
    void learn_only_middle(value_vector const &input_vec, float noise)
    {
        assert(d_layers.size() >= 4);
        
        //activate until middle
        uint32_t before_middle = d_layers.size() / 2 - 2;
        input(input_vec);
        NeuralNetwork::activate(0, before_middle);
        
        //input to middle layers
        std::vector<float> input_middle(d_layers[before_middle].output());

        //add noise over output before middle
        d_layers[before_middle].output(vector_salt_pepper_noise(input_middle, noise / 2.0, noise / 2.0));

        //activate rest
        uint32_t end_middle = d_layers.size() / 2;
        NeuralNetwork::activate(end_middle - 1, end_middle);
        
        d_diff = vector_difference(d_layers[end_middle].output(), input_middle);
        bool relative(true);
        back_propagate_middle(d_diff, d_learn_rate, relative);
    }
    
    float get_diff_norm()
    {
        return vector_norm(d_diff);
    }
    
    void save(std::string filename);
    void load(std::string filename);

  private:
};

template <>
inline void byte_write(std::ostream &out, AutoEncoder const &encoder)
{
    std::cout << "saving encoder" << std::endl;
    byte_write<NeuralNetwork>(out, encoder);
    byte_write(out, encoder.d_learn_rate);
}

template <>
inline AutoEncoder byte_read<AutoEncoder>(std::istream &in)
{
    AutoEncoder encoder = byte_read<NeuralNetwork>(in);
    
    encoder.d_learn_rate = byte_read<float>(in);
    return encoder;
}

inline void AutoEncoder::save(std::string filename)
{
  std::ofstream out(filename.c_str());
  byte_write<AutoEncoder>(out, *this);
}

inline void AutoEncoder::load(std::string filename)
{
  std::ifstream in(filename.c_str());
  *this = byte_read<AutoEncoder>(in);
}

#endif
