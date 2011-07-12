#ifndef __GUIDING_STRATEGY__
#define __GUIDING_STRATEGY__

class GuideStrategy {
public:
    enum SelectType {
        USE_TRAIN,
        USE_TEST,
        USE_ALL
    };

private:
    float d_reward_sigma;
    SelectType d_select_type;
    bool d_use_pos;
public:
    GuideStrategy(float reward_sigma, SelectType select_type, bool use_pos) : 
        d_reward_sigma(reward_sigma),
        d_select_type(select_type),
        d_use_pos(use_pos) {}

    virtual int select_index(int i) {return i;}
    virtual bool use_all() {return d_select_type == USE_ALL;}
    virtual bool use_train() {return d_select_type == USE_TRAIN;}

    virtual FocusState create_start_state(DataElement &data_element, float x, float y, float radius)
    {
        return FocusState(x, y, radius);
    }
    
    virtual std::vector<float> get_encoding(AutoEncoder &auto_encoder, std::vector<float> &current_data, FocusState &state)
    {
        std::vector<float> encoding = auto_encoder.get_encoding(current_data);
        if (d_use_pos)
            {
                float x = state.pos_x - 0.5;  //should lie between -0.5, 0.5
                float y = state.pos_y - 0.5;
                encoding.push_back(x);
                encoding.push_back(y);
            }
        return encoding;
    }

    virtual bool use_noise(){return false;}

    virtual bool use_learning(){return false;}

    virtual void last_step_hook(Focus &focus, FocusState &start_state, FocusState &end_state, AutoEncoder &auto_encoder, float start_reward, float end_reward, float start_distance, float end_distance, size_t index){}
    
    virtual void end_epoch_hook(){}

    float get_reward(FocusState &correct_state, FocusState &current_state)
    {
        return get_state_performance(correct_state, current_state, d_reward_sigma);
    }
};

class TrainGuideStrategy : public GuideStrategy
{
    std::vector<int> d_index_map;
    float d_gradual_start_deviation;

public:
    TrainGuideStrategy(float reward_sigma, std::vector<int> index_map, float gradual_start_deviation, bool use_pos, SelectType select_type = USE_TRAIN) :
        GuideStrategy(reward_sigma, select_type, use_pos),
        d_index_map(index_map), d_gradual_start_deviation(gradual_start_deviation)
    {}
    
    int select_index(int i) {return d_index_map[i];}
    FocusState create_start_state(DataElement &data_element, float x, float y, float radius)
    {
        return FocusState(x + get_random_uniform(0.0, d_gradual_start_deviation), y + get_random_uniform(0, d_gradual_start_deviation), radius);
    }
    
    bool use_noise(){return true;}

    bool use_learning(){return true;}

};

class TestGuideStrategy : public GuideStrategy
{
    float d_start_deviation;
public:
    Mean<float> reward_increase_mean,
        start_distance_mean,
        end_distance_mean,
        distance_increase_mean;
    
    std::vector<float> d_bins;
    std::vector<size_t> d_bin_counter;
    size_t d_counter;
public:
 TestGuideStrategy(float reward_sigma, float start_deviation, std::vector<float> bins, bool use_pos, SelectType select_type = USE_TEST) :
    
    GuideStrategy(reward_sigma, select_type, use_pos),
        d_start_deviation(start_deviation),
        d_bins(bins),
        d_bin_counter(bins.size() + 1),
        d_counter(0)
            {}
    
    FocusState create_start_state(DataElement &data_element, float x, float y, float radius)
    {
        while (true) {
            float angle = get_random_uniform(0.0, 3.142);
            float diff_x = d_start_deviation * cos(angle);
            float diff_y = d_start_deviation * sin(angle);
            FocusState new_state(x + diff_x, y + diff_y, radius);
            
            if (new_state.pos_x > 0.0 && new_state.pos_x < 1.0 &&
                new_state.pos_y > 0.0 && new_state.pos_y < 1.0)
                return new_state;
        }
    }

    
    bool use_noise(){return false;}

    bool use_learning(){return false;}

    void last_step_hook(FocusState &start_state, FocusState &end_state, float start_reward, float end_reward, float start_distance, float end_distance)
    {
        reward_increase_mean.update(end_reward - start_reward);
        start_distance_mean.update(start_distance);
        end_distance_mean.update(end_distance);
        distance_increase_mean.update(end_distance - start_distance);

        d_counter++;

        for (size_t i(0); i < d_bins.size(); ++i)
            {
                if (end_distance < d_bins[i])
                    {
                        d_bin_counter[i]++;
                        return;
                    }
            }
        d_bin_counter[d_bins.size()]++; //wasnt any of the other bins, so it is the last bin
            
    }

    void end_epoch_hook()
    {
    }

    void print_basic()
    {
        log2("reward increase: ", reward_increase_mean.mean());
        log2("start distance: ", start_distance_mean.mean() * 64.);
        log2("end distance: ", end_distance_mean.mean() * 64.);
        log2("distance increase: ", distance_increase_mean.mean() * 64.);

    }

    void print_bins()
    {
        for (size_t i(0); i < d_bins.size(); ++i)
            {
                log2(std::string("closer than | ") + t_to_string(d_bins[i] * 64.) + ": ", (float)d_bin_counter[i] / d_counter);
            }
        log2("last bin : ", (float)d_bin_counter[d_bins.size()] / d_counter);
    }
};

void guiding_algorithm(GuideStrategy &guide_strategy, SmartVector<DataElement> &collection, std::vector<bool> &selection, AutoEncoder &auto_encoder,  FocusSystem &focus_system, std::string face_part, size_t n_action_steps)
{
    for (size_t i(0); i < collection.size(); ++i)
        {
            int index = guide_strategy.select_index(i);
            if (!guide_strategy.use_all())
                if ((!selection[index] && guide_strategy.use_train()) ||
                    (selection[index] && !guide_strategy.use_train()))
                    continue;
            
            DataElement &data_element(*collection[index]);
            
            Focus focus(data_element);
            float correct_x = data_element.get_meta<float>(face_part + "_x");
            float correct_y = data_element.get_meta<float>(face_part + "_y");
            float radius(10);
            
            FocusState correct_state(correct_x, correct_y, radius);
            FocusState start_state(guide_strategy.create_start_state(data_element, correct_x, correct_y, radius));
            FocusState current_state(start_state);
            
            vector<float> current_data = focus.focus_vector(current_state);
            vector<float> current_encoding = guide_strategy.get_encoding(auto_encoder, current_data, current_state);
            
            
            float start_distance = state_distance(correct_state, current_state);                
            float start_reward = guide_strategy.get_reward(correct_state, current_state);
                
            //start_distance_mean.update(start_distance);
            
            for (size_t n(0); n < n_action_steps; ++n) {
                vector<float> chosen_action(2);
                bool use_noise(guide_strategy.use_noise());
                FocusState new_state = focus_system.next_state(current_state, current_encoding, &chosen_action, use_noise);
                //log2("action ", t_to_string(chosen_action));
                //if (epoch < 10)
                //new_state = focus_system.next_noisy_state(current_state, &chosen_action);
                    
                vector<float> new_data = focus.focus_vector(new_state);
                vector<float> new_encoding = guide_strategy.get_encoding(auto_encoder, new_data, new_state);
                
                float reward = guide_strategy.get_reward(correct_state, new_state);
                
                bool last_step = n == (n_action_steps - 1);
                
                if (guide_strategy.use_learning())
                    focus_system.learn(current_encoding, new_encoding, chosen_action, reward, last_step);
                
                current_state = new_state;
                current_data = new_data;
                current_encoding = new_encoding;
                if (last_step) {
                    FocusState end_state(new_state);
                    float end_reward = reward;
                    float end_distance = state_distance(correct_state, new_state);
                    guide_strategy.last_step_hook(focus, start_state, end_state, auto_encoder, start_reward, end_reward, start_distance, end_distance, index);
                    
                }
            }
            
            //reward_increase.update(last_reward - start_reward);
        }
 
    guide_strategy.end_epoch_hook();
}

#endif
