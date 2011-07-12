#include "autoencoder.h"

AutoEncoder::value_vector AutoEncoder::decode(value_vector const &input_values)
{
    
    uint32_t index(d_layer_sizes.size() / 2);
    d_layers[index - 1].output(input_values);
    NeuralNetwork::activate(index);
    return output();
}
