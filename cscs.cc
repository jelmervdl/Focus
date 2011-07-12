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

vector<float> reconstruct_signal(AutoEncoder &cscs, vector<float> &hidden, size_t n)
{
  vector<float> signal(n);

  for (int i(n - 1); i >= 0; --i) {
    vector<float> reconstruct = cscs.decode(hidden);
    copy(reconstruct.begin() + 1, reconstruct.end(), hidden.begin());
    float reconstruct_val = reconstruct[0];

    signal[i] = reconstruct_val;
  }
  return signal;
}

float test_cscs(AutoEncoder &cscs)
{
  float t = 0.0;

  vector<float> hidden(3);
  vector<float> signal;
  for (size_t i(0); i < 1000; ++i) {
    float val = sin(t);
    val = val > 0.0 ? val : 0.0;
    
    vector<float> input(4);
    input[0] = val;
    copy(hidden.begin(), hidden.end(), input.begin() + 1);
    
    vector<float> output = cscs.activate(input);
    hidden = cscs.get_encoding();
    
    if (i > 900) {
      signal.push_back(val);
    }

    t += t_diff;
  }

  vector<float> new_signal = reconstruct_signal(cscs, hidden, signal.size());
  
  return ;
}

//Presenter presenter;
void do_stuff()
{
  log("setting up autoencoder");
  AutoEncoder auto_encoder;
  auto_encoder.setup(string_to_vector<unsigned int>(string("[4,3,4]")));
  auto_encoder.set_learning_rate(0.0001);
  vector<float> hidden(3);

  float t = 0.0;
  size_t index(0);
  while (true) {
    float val = sin(t);
    val = val > 0.0 ? val : 0.0;

    vector<float> input(4);
    input[0] = val;
    copy(hidden.begin(), hidden.end(), input.begin() + 1);
    auto_encoder.learn(input, input, true);
    
    if (index && index % 100000 == 0)
      cout << val << " " << auto_encoder.output()[0] << endl;
    hidden = auto_encoder.get_encoding();

    if (index && index % 10000000 == 0)
      cout << test_cscs(auto_encoder);
    t += t_diff;
    ++index;
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
