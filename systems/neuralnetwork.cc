#include "neuralnetwork.h"

void NeuralNetwork::save(std::string filename)
{
  std::ofstream out(filename.c_str());
  byte_write<NeuralNetwork>(out, *this);
}

void NeuralNetwork::load(std::string filename)
{
  std::ifstream in(filename.c_str());
  *this = byte_read<NeuralNetwork>(in);
}

void NeuralNetwork::input(value_vector const &some_input)
{
    //log(t_to_string(d_layer_sizes[0]), EXTRA);
    //log(some_input.size(), EXTRA);
    
    assert(d_layer_sizes[0] == some_input.size());
    d_layers[0].input(some_input);
}

void NeuralNetwork::back_propagate_middle(value_vector const &diff, float learn_factor, bool relative_learning_rate)
{
  uint32_t middle = d_layers.size() / 2;
  d_layers[middle].back_propagate(diff, learn_factor, relative_learning_rate);
  d_layers[middle - 1].back_propagate_layer(d_layers[middle], learn_factor, relative_learning_rate);
}
  
void NeuralNetwork::setup(std::vector<uint32_t> layer_sizes, bool last_layer_linear, NeuralLayer::InitTypes init_type)
{
    d_layer_sizes = layer_sizes;
    
    assert(d_layer_sizes.size() >= 2);

    d_layers.clear();
    for (uint32_t i(1); i < layer_sizes.size(); ++i)
    {
        bool linear_layer = last_layer_linear && (i == layer_sizes.size() - 1);
        d_layers.push_back(NeuralLayer(layer_sizes[i - 1], layer_sizes[i], linear_layer));
    }
    init(init_type);
}
