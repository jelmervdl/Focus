#ifndef __GENERAL1__
#define __GENERAL1__

//file for general functions
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <iostream>
#include <iterator>
#include <algorithm>
#include <numeric>
#include <ios>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <iomanip>
#include <ctype.h>

#include "smart_pointer.h"
#include "../constants.h"

#include "errors/errors_package.h"
#include "bytewrite.h"
#include "log.h"

typedef std::map<std::string, std::string> string_map;
typedef std::pair<std::string, std::string> string_pair;

inline float max(float a, float b)
{
    return a >= b ? a : b;
}

inline float min(float a, float b)
{
    return a < b ? a : b;
}

/*
inline void play_sound(std::string sound_name)
{
    std::string file_name;
    if (sound_name == "ting")
        file_name = "/Users/marijnstollenga/Downloads/SFML-1.6-sdk-macosx-ub32+64 2/samples/bin/datas/pong/ball.wav";
    
    if (file_name.size())
    {
        sf::Music music;
        music.OpenFromFile(file_name);
        music.Play();
    }
}   
*/

inline std::string strip(std::string const &in)
{
    size_t pos = in.find_first_not_of(" \t\n");
    size_t pos_2 = in.find_last_not_of(" \t\n");
    if (pos == std::string::npos)
        return "";
    if (pos_2 == std::string::npos)
        return in.substr(pos);
    return in.substr(pos, pos_2 - pos + 1);
}

inline string_pair split(std::string in, std::string splitter)
{
  size_t pos = in.find(splitter);
  if (pos == std::string::npos)
    return string_pair(in, "");
  return string_pair(strip(in.substr(0, pos)), strip(in.substr(pos + splitter.size())));
}

inline bool contains(std::string const &in, std::string const &check)
{
    return in.find(check) != std::string::npos;
}

inline std::string read_line(std::istream &in)
{
    std::string return_string;
    while (1)
    {
        char c;
        in.get(c);
        if (in.eof() || c == '\n')
            break;
        return_string += c;
    }
    return return_string;
}

inline std::string get_temporary_filename()
{
    return tmpnam(NULL);
}


template <typename T>
T vector_dot(std::vector<T> const &v1, std::vector<T> const &v2)
{
    T totaal(0.0);
    
    typename std::vector<T>::const_iterator end(v1.end());
    typename std::vector<T>::const_iterator it_1(v1.begin());
    typename std::vector<T>::const_iterator it_2(v2.begin());
    
    for (; it_1 != end; ++it_1, ++it_2)
        totaal += *it_1 * *it_2;
    return totaal;
}

template <typename T>
std::vector<T> vector_difference(std::vector<T> const &v1, std::vector<T> const &v2)
{
  assert(v1.size() == v2.size());
  std::vector<T> result(v1);
  typename std::vector<T>::const_iterator it_1(v1.begin()),
    end_1(v1.end()), it_2(v2.begin());
  typename std::vector<T>::iterator it_result(result.begin());

  for (; it_1 != end_1; ++it_1, ++it_2, ++it_result)
      *it_result = *it_1 - *it_2;
  return result;
}

template <typename T>
std::vector<T> vector_difference(std::vector<T> const &v1, float const &f2)
{
  std::vector<T> result(v1);
  typename std::vector<T>::const_iterator it_1(v1.begin()),
    end_1(v1.end());
  typename std::vector<T>::iterator it_result(result.begin());

  for (; it_1 != end_1; ++it_1, ++it_result)
      *it_result = *it_1 - f2;
  return result;
}

template <typename T>
T vector_norm(std::vector<T> const &v1)
{
  typename std::vector<T>::const_iterator it_1(v1.begin()), end_1(v1.end());

  T total(0.0);

  for (; it_1 != end_1; ++it_1)
      total += *it_1 * *it_1;
  total /= v1.size();
  total = sqrt(total);
  return total;
}

template <typename T>
std::vector<T> normalize_vector(std::vector<T> const &v1)
{
    T length = vector_norm(v1);
  
    return vector_mult(v1, 1.0 / length);
}

template <typename T>
std::vector<T> normalize_std(std::vector<T> const &v1, float std_1)
{
    float mean = vector_mean(v1);
    float stdev = vector_std(v1);


    return vector_mult(vector_add(v1, -mean), std_1 / stdev);
}

template <typename T>
std::vector<T> normalize_vector(std::vector<T> const &v1, float result_min, float result_max)
{
    float min_value(*std::min_element(v1.begin(), v1.end()));
    float max_value(*std::max_element(v1.begin(), v1.end()));
    
    return vector_add(vector_mult(vector_add(v1, -min_value)
                                  , (result_max - result_min) / (max_value - min_value)), result_min);
    
}

template <typename T>
T vector_mean(std::vector<T> const &v1)
{
  return std::accumulate(v1.begin(), v1.end(), 0.0) / v1.size();
}

template <typename T>
T vector_std(std::vector<T> const &v1)
{
  return vector_norm(vector_difference(v1, vector_mean(v1)));
}


template <typename T>
std::vector<T> vector_concat(std::vector<T> const &v1, std::vector<T> const &v2)
{
    std::vector<T> new_vec(v1);
    std::copy(v2.begin(), v2.end(), std::back_inserter(new_vec));
    return new_vec;
}

template <typename T>
std::vector<T> vector_add(std::vector<T> const &v1, std::vector<T> const &v2)
{
    std::vector<T> new_vec(v1);
    for (size_t i(0); i < v1.size(); ++i)
        new_vec[i] = v1[i] + v2[i];
    return new_vec;
}

template <typename T>
std::vector<T> vector_add(std::vector<T> const &v1, float v2)
{
    std::vector<T> new_vec(v1);
    for (size_t i(0); i < v1.size(); ++i)
        new_vec[i] = v1[i] + v2;
    return new_vec;
}

template <typename T>
std::vector<T> vector_mult(std::vector<T> const &v1, float v2)
{
    std::vector<T> new_vec(v1);
    for (size_t i(0); i < v1.size(); ++i)
        new_vec[i] = v1[i] * v2;
    return new_vec;
}


template <typename T>
std::vector<T> vector_salt_pepper_noise(std::vector<T> const &v1, float salt_probability, float pepper_probability)
{
  std::vector<T> new_v(v1);
  
  std::vector<bool> pepper_vec(v1.size(), false);
  std::vector<bool> salt_vec(v1.size(), false);
  
  std::fill(pepper_vec.begin(), pepper_vec.begin() + pepper_vec.size() * pepper_probability, true);
  std::fill(salt_vec.begin(), salt_vec.begin() + salt_vec.size() * salt_probability, true);
  std::random_shuffle(pepper_vec.begin(), pepper_vec.end());
  std::random_shuffle(salt_vec.begin(), salt_vec.end());
  
  typename std::vector<T>::iterator it(new_v.begin()), it_end(new_v.end());
  typename std::vector<bool>::const_iterator salt_it(salt_vec.begin()), pepper_it(pepper_vec.begin());
  
  for (; it != it_end; ++it, ++salt_it, ++pepper_it)
    *it = *pepper_it ? picture_to_float_min : (*salt_it ? picture_to_float_max : *it);

  return new_v;
}

template <typename T>
inline std::vector<T> string_to_vector(std::string in)
{
  size_t start = in.find("[");
  size_t end = in.find("]");
  
  if (start == std::string::npos || end == std::string::npos)
  {
    std::vector<T> result_vector(1);
    result_vector[0] = string_to_t<T>(in);
    return result_vector;
  }
  
  std::string temp_string = in.substr(start + 1, end - start - 1);
  
  std::vector<T> result_vector;

  try {
    while (true)
      {
        string_pair a_pair(split(temp_string, ","));
        result_vector.push_back(string_to_t<T>(a_pair.first));
        
        if (a_pair.second.size() > 0)
          temp_string = a_pair.second;
        else
          break;
      }
  }
  catch(...)
    {
      throw std::runtime_error("couldnt string_to_vector");
    }
  return result_vector;
}

template <typename T>
inline T &last(std::vector<T> &some_vec)
{
    return *(some_vec.end() - 1);
}

template <typename T>
inline T const &last(std::vector<T> const &some_vec)
{
    return *(some_vec.end() - 1);
}

inline std::string read_type(std::istream &in, char t)
{
    if (in.peek() != t)
    {
        std::cerr << "expected " << t << std::endl;
        exit(1);
    }
    std::string return_string;
    return_string += byte_read<char>(in);
    return return_string;
}

inline std::string read_type(std::istream &in, int (*func)(int))
{
    std::string return_string;
    while (1)
    {
        char c = in.peek();
        if (in.eof() || !func(c))
            break;
        return_string += byte_read<char>(in);
    }
    return return_string;
}

inline void copy_stream(std::istream &in, std::ostream &out)
{
    std::istreambuf_iterator<char> i_it(in);
    std::istreambuf_iterator<char> i_end;
    
    std::ostream_iterator<char> o_it(out);
    
    std::copy(i_it, i_end, o_it);
}

inline int make_dir(std::string dir_name)
{
    std::string command = std::string("mkdir -p ") + dir_name;
    return system(command.c_str());
}

inline int remove_file(std::string filename)
{
    std::string command = std::string("rm -f ") + filename;
    return system(command.c_str());
}

class PamImage;
void show_image(PamImage image, std::string savefile = "");

void show_image(std::vector<float> const &v, std::string savefile = "", unsigned int width = 0, unsigned int height = 0);

std::vector<std::string> ls(std::string dir);

template <typename T>
inline T random_value(T min_value, T max_value)
{
    T random_value = (max_value - min_value) * (1.0 * rand() / RAND_MAX) + min_value;
    return random_value;
}

template <typename T>
class Mean
{
    T d_total;
    unsigned int d_n;
 
  public:    
    Mean() : d_total(0), d_n(0){}
    void update(T t){d_total += t, ++d_n;}
    T mean(bool do_reset = false){float a_mean = d_total / d_n; if (do_reset) reset(); return a_mean;}
    void reset(){d_total = 0; d_n = 0;}
};

template <typename T>
class RunningMean
{
 public:
    std::vector<T> d_values;
    unsigned int d_max_n;
 public:    
    
 RunningMean(unsigned int max_n) : d_max_n(max_n) {}
    void update(T t){d_values.push_back(t); if (d_values.size() > d_max_n) d_values.erase(d_values.begin());}
    T mean(bool do_reset = false){float a_mean = vector_mean(d_values); if (do_reset) reset(); return a_mean;}
    T mean_first_half(bool do_reset = false){float a_mean = vector_mean(std::vector<float>(d_values.begin(), d_values.begin() + d_values.size() / 2)); if (do_reset) reset(); return a_mean;}
    T mean_second_half(bool do_reset = false){float a_mean = vector_mean(std::vector<float>(d_values.begin() + d_values.size() / 2, d_values.end())); if (do_reset) reset(); return a_mean;}

    T std(){return vector_std(d_values);}
    void reset(){d_values.clear();}
    bool full(){return d_max_n == d_values.size();}
};

#endif
