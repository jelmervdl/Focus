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
#include <math.h>

using namespace std;

Context context;
float const t_diff(0.1);
size_t const n_hidden(5);
float const learning_rate(0.005);

vector<float> reconstruct_signal(AutoEncoder &scsc, vector<float> &hidden, size_t n)
{
  vector<float> signal(n);

  for (int i(n - 1); i >= 0; --i) {
    vector<float> reconstruct = scsc.decode(hidden);
    copy(reconstruct.begin() + 1, reconstruct.end(), hidden.begin());
    float reconstruct_val = reconstruct[0];

    signal[i] = reconstruct_val;
  }
  return signal;
}

float test_scsc(AutoEncoder &scsc, bool and_print = false)
{
  float t = 0.0;

  vector<float> hidden(n_hidden);
  vector<float> signal;
  for (size_t i(0); i < 1000; ++i) {
    float val = sin(t);
    val = val > 0.0 ? val : 0.0;
    
    vector<float> input(n_hidden + 1);
    input[0] = val;
    copy(hidden.begin(), hidden.end(), input.begin() + 1);
    
    vector<float> output = scsc.activate(input);
    hidden = scsc.get_encoding();
    
    if (i > 900) {
      signal.push_back(val);
    }

    t += t_diff;
  }


  vector<float> new_signal = reconstruct_signal(scsc, hidden, signal.size());
  
  if (and_print)
    for (size_t i(0); i < signal.size(); ++i)
      cout << "|" << signal[i] << " " << new_signal[i] << endl;
  return vector_norm(vector_difference(signal, new_signal));
}

//Presenter presenter;
void do_stuff()
{
  log("setting up autoencoder");
  AutoEncoder auto_encoder;
  auto_encoder.setup(string_to_vector<unsigned int>(string("[") + t_to_string(n_hidden + 1) +"," + t_to_string(n_hidden) + "," + t_to_string(n_hidden + 1) + "]"));
  auto_encoder.set_learning_rate(learning_rate);
  vector<float> hidden(n_hidden);

  float t = 0.0;
  size_t index(0);
  float lowest(1.0);

  RunningMean<float> running_value_error(100);
  RunningMean<float> running_hidden_error(100);
  
  vector<float> last_input(n_hidden + 1);
  log("starting");
  while (true) {
    float val = sin(t);
    val = val > 0.0 ? val : 0.0;

    vector<float> input(n_hidden + 1);
    input[0] = val;
    copy(hidden.begin(), hidden.end(), input.begin() + 1);
    auto_encoder.learn(input, input, true);

    //auto_encoder.activate(input);
    running_value_error.update(vector_norm(vector<float>(auto_encoder.diff().begin(), auto_encoder.diff().begin() + 1)));
    running_hidden_error.update(vector_norm(vector<float>(auto_encoder.diff().begin() + 1, auto_encoder.diff().end())));

    //    if (index && index % 100000 == 0)
    //  cout << "vals: " << val << " " << auto_encoder.output()[0] << endl;
    //hidden = auto_encoder.get_encoding();

    //extra learn TRYOUT
    //vector<float> past_hidden = vector<float>(auto_encoder.output().begin() + 1, auto_encoder.output().end());
    //vector<float> output = auto_encoder.decode(past_hidden);
    //output[0] = last_input[0];
    //auto_encoder.layers()[1].back_propagate(vector_difference(auto_encoder.output(), output), learning_rate, true);
    //auto_encoder.learn(last_input, last_input, true);

    if (index && index % 100000 == 0)
      {
	float test_value = test_scsc(auto_encoder);
	if (lowest > test_value) {
	  lowest = test_value;
	  //test_scsc(auto_encoder, true);
	  //	  cout << "current lowest: " << lowest << endl;

	}
	cout << test_value << "  "
 	     << running_value_error.mean() << "  "
	     << running_hidden_error.mean() << "  "
	     << running_value_error.std() << "  "
	     << running_hidden_error.std() << endl;
      }
    t += t_diff;
    ++index;
    last_input = input;
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
