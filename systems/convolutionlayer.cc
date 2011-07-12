#include "convolutionlayer.h"

ConvolutionLayer::ConvolutionLayer()
{
    d_ae.setup(string_to_vector<unsigned int>("[16, 16, 16]"));
    d_ae.layers()[0].d_l1_factor = 0.01;
    d_ae.set_learning_rate(0.001);
}

void ConvolutionLayer::learn(DataElement &dataelement)
{
  int width = dataelement.get_meta<int>("width");
  int height = dataelement.get_meta<int>("height");
  
  Focus focus(dataelement);
  
  uint32_t sub_width(4);
  uint32_t sub_height(4);
  for (size_t x(0); x <= width - sub_width; x += sub_width)
      for (size_t y(0); y <= height - sub_height; y += sub_height)
      {
          std::vector<float> sub_vec(focus.sub_image(x, y, sub_width, sub_height));
          d_ae.learn(sub_vec);
      }
    d_mean.update(d_ae.get_diff_norm());
}

ConvolutionLayer::value_vector ConvolutionLayer::encode(DataElement &dataelement)
{
  value_vector return_vec;
  
  int width = dataelement.get_meta<int>("width");
  int height = dataelement.get_meta<int>("height");
  
  Focus focus(dataelement);
  
  uint32_t sub_width(4);
  uint32_t sub_height(4);
  for (size_t x(0); x <= width - sub_width; x += sub_width)
      for (size_t y(0); y <= height - sub_height; y += sub_height)
      {
          std::vector<float> sub_vec(focus.sub_image(x, y, sub_width, sub_height));
          return_vec = vector_concat(return_vec, d_ae.get_encoding(sub_vec));
      }
  return return_vec;
}

void ConvolutionLayer::decode(value_vector &values, DataElement *dataelement_ptr)
{
  DataElement &dataelement(*dataelement_ptr);
  
  int width = dataelement.get_meta<int>("width");
  int height = dataelement.get_meta<int>("height");
  
  Focus focus(dataelement);
  
  uint32_t sub_width(4);
  uint32_t sub_height(4);
  uint32_t stride = values.size() / ((width * height) / (sub_height * sub_height));
  value_vector::iterator v_it(values.begin());
  
  for (size_t x(0); x <= width - sub_width; x += sub_width)
      for (size_t y(0); y <= height - sub_height; y += sub_height, v_it += stride)
      {
          value_vector sub_vec(v_it, v_it + stride);
          
          value_vector decoding = d_ae.decode(sub_vec); 
          
          //log(t_to_string(sub_vec), EXTRA);
          //log(t_to_string(decoding), EXTRA);
          focus.set_sub_image((int)x, (int)y, (int)sub_width, (int)sub_height, decoding);
          
      }
  dataelement.data = focus.pixels();
}
