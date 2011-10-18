#include "neurallayer.h"

NeuralLayer::value_vector &NeuralLayer::input()
{
  return d_input_activation;  
}

void NeuralLayer::init(InitTypes init_type)
{
    log("initing", 2);
    value_vector::iterator weight_it(d_weights.begin()), weight_end(d_weights.end());
    value_vector::iterator bias_it(d_bias.begin()), bias_end(d_bias.end());
    
    switch (init_type) {
    case RANDOM:
        for (;weight_it != weight_end; ++weight_it)
            {
                *weight_it = get_random_uniform(0, 0.3 / sqrt(d_n_inputs));//sqrt((float) d_input_activation.size()));
                log(t_to_string<>(*weight_it), TRIVIAL);
            }
        for (;bias_it != bias_end; ++bias_it)
            {
                *bias_it = get_random_uniform(0, 0.001);
                log(t_to_string<>(*bias_it), TRIVIAL);
            }
        
        break;
    case ZERO:
        fill(d_weights.begin(), d_weights.end(), 0.0);
        fill(d_bias.begin(), d_bias.end(), 0.0);
        break;
    }
}

void NeuralLayer::activate()
{
    value_vector::iterator
        output_it(d_output_activation.begin()), // CHANGES
        output_end(d_output_activation.end());

    value_vector::const_iterator bias_it(d_bias.begin());
    
    uint32_t output_n(0);
    float *weight_it = &weight(0, 0);
    
    for (;output_it != output_end; ++output_it, ++bias_it, ++output_n)
    {
        value_vector::const_iterator
            input_it(d_input_activation.begin()),
            input_end(d_input_activation.end());
        
        weight_type activation(*bias_it);
        
        for (;input_it != input_end; ++input_it, ++weight_it)
        {
            activation += *input_it * *weight_it;
        }

        if (!d_linear_layer)
            activation = kernel(activation);
        *output_it = activation;
    }
}

void NeuralLayer::back_propagate(value_vector const &diff, weight_type learn_factor, bool use_relative_learning_rate)
{
    assert(diff.size() == d_n_outputs);
    
    bool const weights_are_locked(d_weights_are_locked);
    
    weight_type const learning_rate(use_relative_learning_rate
        ? learn_factor / sqrt(d_input_activation.size())
        : learn_factor);

    value_vector norm_diff(diff);

    // d-l1-factor has to do with the sparse encoding thingy.
    if (d_l1_factor != 0.0)
    {
        float
            dot_product(0.0),
            l1_length(0.0),
            adjust_length(0.0);
        
        value_vector::const_iterator
            output_it(d_output_activation.begin()),
            output_end(d_output_activation.end());
        
        value_vector::iterator diff_it(norm_diff.begin()); // CHANGES
        
        for (; output_it != output_end; ++output_it, ++diff_it)
        {
            float l1_adjust = (*output_it > 0.0)
                ? d_l1_factor
                : -d_l1_factor;
            
            adjust_length += *diff_it * *diff_it;
            l1_length += l1_adjust * l1_adjust;
            dot_product += l1_adjust * *diff_it;
            
            *diff_it += l1_adjust;
        }
        
        dot_product /= sqrt(l1_length) * sqrt(adjust_length);
        d_l1_adjust_angle.update(dot_product); // CHANGES
    }
    
    value_vector::const_iterator
        output_it(d_output_activation.begin()),
        output_end(d_output_activation.end());
    
    value_vector::const_iterator diff_it(norm_diff.begin());
    
    value_vector::iterator bias_it(d_bias.begin()); // CHANGES
    
    float *weight_it = &weight(0, 0); // CHANGES
    
    std::fill(d_adjustments.begin(), d_adjustments.end(), 0.0);

    for (;output_it != output_end; ++output_it, ++diff_it, ++bias_it)
    {
        weight_type temp_adjustment = d_linear_layer
            ? (*diff_it)
            : (*diff_it) * (1.0 - (*output_it) * (*output_it)); //linear or tanh layer?
        
        if (!weights_are_locked)
            *bias_it -= 0.1 * learning_rate * (temp_adjustment);
    
        value_vector::const_iterator
            input_it(d_input_activation.begin()),
            input_end(d_input_activation.end());
        
        value_vector::iterator adjustment_it(d_adjustments.begin()); // CHANGES
    
        for (;input_it != input_end; ++input_it, ++weight_it, ++adjustment_it)
        {
            *adjustment_it += temp_adjustment * *weight_it;
            
            if (weights_are_locked)
                continue;
            
            weight_type const adjust = learning_rate * (temp_adjustment * (*input_it));    
            *weight_it -= adjust;
        }
    }
}
