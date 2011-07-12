//#define DLIB_ISO_CPP_ONLY

#include "misc/misc_package.h"
#include "systems/systems_package.h"
#include "resources/resources_package.h"
#include "image/image_package.h"

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

//Presenter presenter;

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

void do_stuff()
{
  string resource_dir_name(context.get<string>("dir"));
  string resource_name("annotated");
  string resource_type("normal");
    
  string save_dir(context.get<string>("save_dir"));
  string hidden_layers(context.get<string>("hidden_layers"));
  size_t n_epochs(context.get<size_t>("n_epochs"));
    
  float learning_rate(context.get<float>("learning_rate"));
  float l1_constant(context.get<float>("l1"));
  float noise(context.get<float>("noise"));

  log("start loading");
  CollectionManager cm = CollectionManager::get_instance();
  cm.add_collection(resource_name, resource_dir_name, resource_type);
  log("collection added");
  smart_pointer<Collection> collection_ptr = cm.get_collection(resource_name);
  log("got pointer");
        
  SmartVector<DataElement> collection = collection_ptr->collection();
  normalize_collection(&collection);

  log("getting select file");

  string select_filename(save_dir + "autoencoder.select");
  create_selection_file(select_filename, collection.size(), 0.666);

  ifstream select_file(select_filename.c_str());
  vector<bool> selection(byte_read_vector<bool>(select_file));

  log("setting up autoencoder");
  AutoEncoder auto_encoder;
  auto_encoder.setup(string_to_vector<unsigned int>(string("[4096, ") + hidden_layers + ", 4096]"));

  auto_encoder.set_middle_l1(l1_constant);
  auto_encoder.print_info();
    
  log2("selection size: ", selection.size());
  log2("collection size: ", collection.size());
  assert(selection.size() == collection.size());
    
  RunningMean<float> running_mean_train_error(30);
    
  for (size_t epoch(0); epoch < n_epochs; ++epoch)
    {
      log2("epoch: ", epoch, EXTRA);
      auto_encoder.set_learning_rate(learning_rate);
        
      std::vector<int> index_map(collection.size());
      for (size_t i(0); i < index_map.size(); ++i)
        index_map[i] = i;
      std::random_shuffle(index_map.begin(), index_map.end());
        
      for (size_t i(0); i < collection.size(); ++i)
        {
          int index = index_map[i];
          if (!selection[index]) //skip test
            continue;
            
          DataElement &data_element(*collection[index]);
          vector<float> data(data_element.data);
          vector<float> noisy_data(vector_salt_pepper_noise(data, noise / 2, noise / 2));
          auto_encoder.learn(noisy_data, data, true);
        }
        
      //calculate train error
      Mean<float> mean_train_error;
      for (size_t i(0); i < collection.size(); ++i)
        {
          if (!selection[i]) //skip test
            continue;
            
          DataElement &data_element(*collection[i]);
          vector<float> data(data_element.data);
          auto_encoder.activate(data);

          float error = auto_encoder.get_diff_norm();
          mean_train_error.update(error);
        }
      //calculate test error
      Mean<float> mean_test_error;
      for (size_t i(0); i < collection.size(); ++i)
        {
          if (selection[i]) //skip train
            continue;
            
          DataElement &data_element(*collection[i]);
          vector<float> data(data_element.data);
          auto_encoder.activate(data);

          float error = auto_encoder.get_diff_norm();
          mean_test_error.update(error);
        }
        
      running_mean_train_error.update(mean_train_error.mean());
      log2("train error: ", mean_train_error.mean());
      log2("test  error: ", mean_test_error.mean());
      auto_encoder.print_l1_adjust_angles();
        
      if (running_mean_train_error.full() && running_mean_train_error.mean_first_half() < running_mean_train_error.mean_second_half())
        {
          log("stagnated, stopping");
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
    
  do_stuff();
    
  return 0;
}
