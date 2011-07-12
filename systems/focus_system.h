#ifndef __FOCUSSYSTEM_H__
#define __FOCUSSYSTEM_H__

#include "../datastructs/datastructs_package.h"
#include "../misc/misc_package.h"
#include "neuralnetwork.h"

class FocusSystem
{
    friend void byte_write<>(std::ostream &out, FocusSystem const &focus_system);
    friend FocusSystem byte_read<FocusSystem>(std::istream &in);
public:
    float d_alpha, d_beta, d_gamma, d_max_step;
    bool d_just_activated;
    
    NeuralNetwork d_V, d_Ac;
    std::vector<float> d_exploration_sigma;
    
    Conditions d_conditions;
    
public:
    FocusSystem()
    :
    d_alpha(0.0),
    d_beta(0.0),
    d_gamma(0.0),
    d_max_step(0.0),
    d_just_activated(false)
    {
        uint32_t action_size(2);
        d_exploration_sigma.resize(action_size);
        
        d_conditions.add_condition("alpha");
        d_conditions.add_condition("beta");
        d_conditions.add_condition("gamma");
        d_conditions.add_condition("max_step");
        d_conditions.add_condition("v_layers");
        d_conditions.add_condition("ac_layers");
    }
    
    void set_position_sigma(float sigma)
    {
        d_exploration_sigma[0] = sigma;
        d_exploration_sigma[1] = sigma;
        
        //d_exploration_sigma[4] = sigma; //hack
        //d_exploration_sigma[5] = sigma; //hack
    }
    
    void set_radius_sigma(float sigma)
    {
        d_exploration_sigma[2] = sigma;
        d_exploration_sigma[3] = sigma;
    }
    
    
    std::vector<float> get_action(std::vector<float> &input_vector, bool with_noise, bool with_action = true)
    {
        d_Ac.activate(input_vector);
        std::vector<float> return_vector(d_Ac.output());

        if (with_noise)
            add_noise(&return_vector);
        d_just_activated = true;
        return return_vector;
    }
    
    void normalize_action(std::vector<float> *action_vector_ptr)
    {
        std::vector<float> &action_vector(*action_vector_ptr);
        float const max_step(d_max_step);
        float const step_length_2(action_vector[0] * action_vector[0] + action_vector[1] * action_vector[1]);
        if (step_length_2 > max_step * max_step)
        {
            action_vector[0] *= max_step / sqrt(step_length_2);
            action_vector[1] *= max_step / sqrt(step_length_2);
        }
    }
    
    FocusState next_state(FocusState &current_state, std::vector<float> &current_encoding, std::vector<float> *action, bool with_noise = false)
    {
        FocusState next_state(current_state);
        *action = get_action(current_encoding, with_noise);
        normalize_action(action);
        next_state.move(*action);
        next_state.sanity_check();
        return next_state;
    }
    
    FocusState next_noisy_state(FocusState &current_state, std::vector<float> *action)
    {
        //log("next noise state", EXTRA);
        FocusState next_state(current_state);
        
        action->resize(d_exploration_sigma.size());
        std::fill(action->begin(), action->end(), 0.0);
        add_noise(action);
        normalize_action(action);
        next_state.move(*action);
        next_state.sanity_check();
        return next_state;
    }
    
    bool ok()
    {
        return d_conditions.ok();
    }
    
    void learn(std::vector<float> const &current_input, std::vector<float> const &next_input, std::vector<float> const &action_vector, float reward, bool last_step = false)
    {   
        d_V.activate(next_input);
        float next_V(d_V.output()[0]);
        
        d_V.activate(current_input);
        float current_V(d_V.output()[0]);
        
        float delta = reward + d_gamma * next_V - current_V;
        /*
        log("==", EXTRA);
        log(reward, EXTRA);
        log(current_V, EXTRA);
        log(next_V, EXTRA);
        log(delta, EXTRA);
        */

        bool use_relative_learningrate(false);
        
        if (!last_step) //if last step -> correct for lost future rewards
            d_V.converge(reward + d_gamma * next_V, d_beta, use_relative_learningrate);
        else
            d_V.converge(reward / (1.0 - d_gamma), d_beta, use_relative_learningrate);
        
        if (delta > 0.0)
        {
            //if (!d_just_activated)
                d_Ac.activate(current_input);
                d_Ac.converge(action_vector, d_alpha, use_relative_learningrate);
        }
        
        d_just_activated = false;
    }
    
 private:
    void add_noise(std::vector<float> *action_vector)
    {   
        assert(action_vector->size() == d_exploration_sigma.size());
        std::vector<float> &change_vector(*action_vector);
        
        for (uint32_t i(0); i < change_vector.size(); ++i)
            change_vector[i] += get_random_normal(0., d_exploration_sigma[i]);
    }
    
 public:
    
    virtual void set(std::string name, std::string value)
    {
        log(name + " " + value, EXTRA);
        
        if (name == "v_layers")
        {
            d_V.setup(string_to_vector<uint32_t>(value), true);
            d_conditions.ok("v_layers", true);
        }
        else if (name == "ac_layers")
        {
            d_Ac.setup(string_to_vector<uint32_t>(value), true);
            d_conditions.ok("ac_layers", true);
        }
        else if (name == "alpha")
        {
            d_alpha = string_to_t<float>(value);
            d_conditions.ok("alpha", true);
        }
        else if (name == "beta")
        {
            d_beta = string_to_t<float>(value);
            d_conditions.ok("beta", true);
        }
        else if (name == "gamma")
        {
            d_gamma = string_to_t<float>(value);
            d_conditions.ok("gamma", true);
        }
        else if (name == "max_step")
        {
            d_max_step = string_to_t<float>(value);
            d_conditions.ok("max_step", true);
        }
        else if (name == "pos_sigma")
        {
            set_position_sigma(string_to_t<float>(value));
        }
        else if (name == "radius_sigma")
        {
            set_radius_sigma(string_to_t<float>(value));
        }
        else
            throw std::runtime_error(std::string("set function: name not found: ") + name);
    }
           
    void save(std::string filename);
    void load(std::string filename);
    
};

template <>
inline void byte_write(std::ostream &out, FocusSystem const &focus_system)
{
    byte_write<>(out, focus_system.d_alpha);
    byte_write<>(out, focus_system.d_beta);
    byte_write<>(out, focus_system.d_gamma);
    byte_write<>(out, focus_system.d_max_step);
    byte_write<>(out, focus_system.d_just_activated);
    
    byte_write<>(out, focus_system.d_V);
    byte_write<>(out, focus_system.d_Ac);
    byte_write_vector<>(out, focus_system.d_exploration_sigma);
}

template <>
inline FocusSystem byte_read<FocusSystem>(std::istream &in)
{
    FocusSystem focus_system;
    
    focus_system.d_alpha = byte_read<float>(in);
    focus_system.d_beta = byte_read<float>(in);
    focus_system.d_gamma = byte_read<float>(in);
    focus_system.d_max_step = byte_read<float>(in);
    focus_system.d_just_activated = byte_read<bool>(in);
    
    focus_system.d_V = byte_read<NeuralNetwork>(in);
    focus_system.d_Ac = byte_read<NeuralNetwork>(in);
    
    focus_system.d_exploration_sigma = byte_read_vector<float>(in);
    return focus_system;
}

float get_state_performance(FocusState &s1, FocusState &s2, float sigma);
float state_distance(FocusState &s1, FocusState &s2);

#endif
