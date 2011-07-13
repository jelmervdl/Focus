#include "misc/misc_package.h"
#include "systems/systems_package.h"
#include "resources/resources_package.h"
#include "image/image_package.h"
#include "datastructs/datastructs_package.h"

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <iterator>
#include <ios>
#include <time.h>

using namespace std;

Context context;

void create_selection_file(std::string filename, unsigned int size, float portion)
{
  vector<bool> selection(size);
  fill(selection.begin(), selection.end(), false);
  fill(selection.begin(), selection.begin() + selection.size() * portion, true);
  random_shuffle(selection.begin(), selection.end());

  ofstream outfile(filename.c_str());
  byte_write_vector(outfile, selection);
}

void normalize_collection(SmartVector<DataElement> *collection_ptr)
{
  SmartVector<DataElement> &collection(*collection_ptr);

  for (size_t i(0); i < collection.size(); ++i)
    collection[i]->data = normalize_std(collection[i]->data, 0.3);
}

SmartVector<DataElement> get_normalized_collection(std::string dir_name)
{
  string resource_dir_name(dir_name);
  string resource_name("annotated");
  string resource_type("normal");

  CollectionManager cm = CollectionManager::get_instance();
    
  cm.add_collection(resource_name, resource_dir_name, resource_type);
  log("collection added");
  smart_pointer<Collection> collection_ptr = cm.get_collection(resource_name);
  log("got pointer");
        
  SmartVector<DataElement> collection = collection_ptr->collection();
  normalize_collection(&collection);
  return collection;
}

float do_stuff()
{
  string resource_dir_name(context.get<string>("dir"));

  string face_part(context.get<string>("face_part"));
  size_t patches_per_face(context.get<size_t>("patches_per_face"));
    
  string save_dir(context.get<string>("save_dir"));
  string n_hidden(context.get<string>("n_hidden"));
  size_t n_epochs(context.get<size_t>("n_epochs"));
    
  float learning_rate(context.get<float>("learning_rate"));
  float learning_rate_threshold(context.get<float>("learning_rate_threshold"));
  float l1_constant(context.get<float>("l1"));
  float noise(context.get<float>("noise"));
  float deviation(context.get<float>("deviation"));
  float test_deviation(0.0);
  float radius(10);

  log("start loading");
  SmartVector<DataElement> collection(get_normalized_collection(resource_dir_name));

  log("getting select file");

  string select_filename(save_dir + "autoencoder.select");
  create_selection_file(select_filename, collection.size(), 2.0 / 3.0);

  ifstream select_file(select_filename.c_str());
  vector<bool> selection(byte_read_vector<bool>(select_file));
  
  log("setting up autoencoder");
  AutoEncoder auto_encoder;
  auto_encoder.setup(string_to_vector<unsigned int>(string("[400, ") + n_hidden + ", 400]"));
  auto_encoder.set_middle_l1(l1_constant);
  auto_encoder.print_info();
    
  log2("selection size: ", selection.size());
  log2("collection size: ", collection.size());
  assert(selection.size() == collection.size());

  RunningMean<float> running_mean_train_error(60);

  //for fixing the x and y pos in the dataset
  vector<float> x_pos(collection.size());
  vector<float> y_pos(collection.size());


  for (size_t epoch(0); epoch < n_epochs; ++epoch)
    {

//      log2("epoch: ", epoch, EXTRA);
      auto_encoder.set_learning_rate(learning_rate);
        
      std::vector<int> index_map(collection.size());
      for (size_t i(0); i < index_map.size(); ++i)
        index_map[i] = i;
      std::random_shuffle(index_map.begin(), index_map.end());
        
      for (size_t i(0); i < collection.size(); ++i) //training
        {
          int index = index_map[i];
          DataElement &data_element(*collection[index]);

          if (!x_pos[index]) {
        	  x_pos[index] = data_element.get_meta<float>(face_part + "_x") + get_random_uniform(0.0, deviation);
        	  y_pos[index] = data_element.get_meta<float>(face_part + "_y") + get_random_uniform(0.0, deviation);
          }
          if (!selection[index]) //skip test
            continue;
            
            
          Focus focus(data_element);


          float x(x_pos[index]);
          float y(y_pos[index]);
          assert(x && y);

          for (size_t n(0); n < patches_per_face; ++n)
            {
              FocusState focus_state(x + get_random_uniform(0.0, deviation), y + get_random_uniform(0, deviation), radius);
              vector<float> data = focus.focus_vector(focus_state);
              vector<float> noisy_data = vector_salt_pepper_noise(data, noise / 2, noise / 2);
              auto_encoder.learn(noisy_data, data);
            }
            
        }
        
      //seperate train test
      Mean<float> mean_train_error;
      for (size_t i(0); i < collection.size(); ++i)
        {
          if (!selection[i]) //skip test
            continue;
            
          DataElement &data_element(*collection[i]);
            
          Focus focus(data_element);
          float x(x_pos[i]);
          float y(y_pos[i]);
          assert(x && y);
          FocusState focus_state(x + get_random_uniform(0.0, test_deviation), y + get_random_uniform(0, test_deviation), radius);
            
          vector<float> data = focus.focus_vector(focus_state);
          auto_encoder.activate(data);

          float error = auto_encoder.get_diff_norm();
          mean_train_error.update(error);
        }
      //test
      Mean<float> mean_test_error;
      for (size_t i(0); i < collection.size(); ++i)
        {
          if (selection[i]) //skip train
            continue;
            
          DataElement &data_element(*collection[i]);
            
          Focus focus(data_element);
          float x(x_pos[i]);
          float y(y_pos[i]);
          assert(x && y);
          FocusState focus_state(x + get_random_uniform(0.0, test_deviation), y + get_random_uniform(0, test_deviation), radius);
          vector<float> data = focus.focus_vector(focus_state);
            
          auto_encoder.activate(data);
            
          float error = auto_encoder.get_diff_norm();
          mean_test_error.update(error);
        }
        
      running_mean_train_error.update(mean_train_error.mean());
//      log2("train error: ", mean_train_error.mean());
//      log2("test  error: ", mean_test_error.mean());

      if (running_mean_train_error.full() && 
          ((running_mean_train_error.mean_first_half() - running_mean_train_error.mean_second_half()) / running_mean_train_error.mean_first_half() < 0.00))
        {
    	  cout << ".";
//          log("stagnated, decreasing");
          learning_rate /= sqrt(2.0);
          running_mean_train_error.reset();
//          log2("new learningrate: ", learning_rate);
          //break;
        }
      if (learning_rate < learning_rate_threshold) {
          log("learning_threshold reached, stopping");
          log2("train error: ", mean_train_error.mean());
          log2("test  error: ", mean_test_error.mean());
          return mean_test_error.mean();
    	  break;
      }
    }
    
  if (save_dir.size())
    {
      log("saving autoencoder");
      auto_encoder.save(save_dir + "autoencoder.save");
    }
    
}

int main(int argc, char **argv)
{
  srand(time(0));
  cout.precision(4);
    
  if (argc % 2 == 0)
    {
      cerr << "need even number of arguments (name value pairs)" << endl;
      return 1;
    }
    
  for (int i(1); i < argc; i += 2) //read name value pairs in argument
    {
      context[argv[i]] = argv[i + 1];
    }


  vector<float> rmse_vec;
  while (true) {
	  rmse_vec.push_back(do_stuff());
	  cout << "mean: " << vector_mean(rmse_vec) << endl;
	  cout << "std:  " << vector_std(rmse_vec) << endl;
	  cout << "size: " << rmse_vec.size() << endl;
  }
  return 0;
}
