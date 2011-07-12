#ifndef __K_AUTO_ENCODER__
#define __K_AUTO_ENCODER__

#include "neuralnetwork.h"
#include "../misc/misc_package.h"
#include <algorithm>
#include <iterator>

class KAutoEncoder// : public NeuralNetwork
{
    friend void byte_write<>(std::ostream &out, KAutoEncoder const &encoder);
    friend KAutoEncoder byte_read<KAutoEncoder>(std::istream &in);
    
    typedef NeuralNetwork::value_vector value_vector;
    float d_learn_rate;

    std::string d_auto_encoder_layers;
    Mean<float> d_mean_error;

  public:
    std::vector<AutoEncoder> d_auto_encoders;
    std::vector<uint32_t> d_win_counter;
    uint32_t d_k, d_winner_index;
    
  public:
    KAutoEncoder() : d_learn_rate(0.0), d_k(0){}  
    
    void set_learning_rate(float learn_rate)
    {
        d_learn_rate = learn_rate;
        for (uint32_t i(0); i < d_auto_encoders.size(); ++i)
            d_auto_encoders[i].set_learning_rate(learn_rate);
    }
    
    void set_layers(std::string layers)
    {
        d_auto_encoder_layers = layers;    
    }
    
    void set_k(uint32_t k)
    {
        d_k = k;
    }
    value_vector const &activate(value_vector const &input_values)
    {
        return activate(input_values, input_values);                            
    }
    
    value_vector const &activate(value_vector const &input_values, value_vector const &original_vector)
    {
        choose_auto_encoder(input_values, original_vector, 0.0);
        return output();
    }
    
    value_vector const &output()
    {
        return d_auto_encoders[d_winner_index].output();    
    }
    
    void learn(value_vector const &input_values, value_vector const &target, float random_chance)
    {
        size_t chosen_index = choose_auto_encoder(input_values, target, random_chance);
        d_auto_encoders[chosen_index].learn(input_values, target);
    }
    
    void learn_only_middle(value_vector const &input_values, value_vector const &target, float random_chance)
    {
        assert(false);
        size_t chosen_index = choose_auto_encoder(input_values, target, random_chance);
        d_auto_encoders[chosen_index].learn_only_middle(input_values, 0.0);
    }
    
    value_vector get_encoding(value_vector const &input_values)
    {
        size_t chosen_index = choose_auto_encoder(input_values, input_values, 0.0);
        return d_auto_encoders[chosen_index].get_encoding();
    }
    
    value_vector get_encoding()
    {
        return d_auto_encoders[0].get_encoding();
    }
    
    value_vector get_input()
    {
        return d_auto_encoders[0].input();
    }
    
    NeuralNetwork::layer_vector get_layers()
    {
        return d_auto_encoders[0].layers();
    }
    
    value_vector get_output()
    {
        return d_auto_encoders[0].output();
    }
    
    
    float encoding_error(AutoEncoder &encoder, std::vector<float> const &values, std::vector<float> const &target)
    {
        encoder.activate(values, target);
        return encoder.get_diff_norm();
    }
    
    std::vector<float> get_feature_vector()
    {
        std::vector<float> return_vector;
        std::vector<float> winner_encoding(d_auto_encoders[d_winner_index].get_encoding());
        
        for (size_t i(0); i < d_k; ++i)
            if (i == d_winner_index)
                std::copy(winner_encoding.begin(), winner_encoding.end(), std::back_inserter<>(return_vector));
            else
            {
                std::vector<float> empty(winner_encoding.size(), 0);
                std::copy(empty.begin(), empty.end(), std::back_inserter<>(return_vector));
            }
        return return_vector;
    }
    
    float mean_error()
    {
        return d_mean_error.mean();
    }
    
    void reset_mean_error()
    {
        d_mean_error.reset();
    }
    
    uint32_t choose_auto_encoder(std::vector<float> const &data, std::vector<float> const &target, float random_chance)
    {
        float min_value(999999.);
        uint32_t min_index(0);
        
        for (uint32_t i(0); i < d_auto_encoders.size(); ++i)
        {
            float error(encoding_error(d_auto_encoders[i], data, target));
            
            if (error < min_value)
            {
                min_value = error;
                min_index = i;
            }
        }
        d_mean_error.update(min_value);
        
        ++d_win_counter[min_index];
        d_winner_index = min_index;
        if (random_value<float>(0.0, 1.0) < random_chance)
            return random_value<uint32_t>(0, d_auto_encoders.size());
        else
            return min_index;
    }
    
    std::vector<AutoEncoder> &encoders()
    {
        return d_auto_encoders;
    }
    
    uint32_t size() {
        return d_auto_encoders.size();
    }
    
    std::vector<uint32_t> &win_counter()
    {
        return d_win_counter;
    }
    
    uint32_t winner()
    {
        return d_winner_index;
    }
    
    void setup()
    {
        d_auto_encoders.clear();
        d_win_counter.clear();
        d_win_counter.resize(d_k);
        
        std::string encoding_layers(d_auto_encoder_layers.size() ? d_auto_encoder_layers : "[1,1,1]");
        log("Encoding Layers:", d_k, EXTRA);
        log(encoding_layers, EXTRA);////
        
        for (uint32_t i(0); i < d_k; ++i)
        {
            AutoEncoder encoder;
            encoder.set_learning_rate(d_learn_rate);
            encoder.setup(string_to_vector<uint32_t>(encoding_layers));
            d_auto_encoders.push_back(encoder);
        }
    }

    void save(std::string filename);
    void load(std::string filename);

};

template <>
inline void byte_write(std::ostream &out, KAutoEncoder const &encoder)
{
    byte_write_vector(out, encoder.d_auto_encoders);
    byte_write(out, encoder.d_auto_encoder_layers);
    byte_write(out, encoder.d_k);
}

template <>
inline KAutoEncoder byte_read<KAutoEncoder>(std::istream &in)
{
    KAutoEncoder k_auto_encoder;
    
    std::vector<AutoEncoder> const &encoders(byte_read_vector<AutoEncoder>(in));
    
    k_auto_encoder.set_layers(byte_read<std::string>(in));
    k_auto_encoder.d_k = byte_read<uint32_t>(in);
    
    log("before setup", EXTRA);
    k_auto_encoder.setup();
    k_auto_encoder.d_auto_encoders = encoders;
    log("done setup", EXTRA);
    return k_auto_encoder;
}

inline void KAutoEncoder::save(std::string filename)
{
  std::ofstream out(filename.c_str());
  byte_write<KAutoEncoder>(out, *this);
}

inline void KAutoEncoder::load(std::string filename)
{
    log("loading k auto encoder", EXTRA);
  std::ifstream in(filename.c_str());
  *this = byte_read<KAutoEncoder>(in);
}

#endif
