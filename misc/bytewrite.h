#ifndef __BYTEWRITE__
#define __BYTEWRITE__

#include <iostream>
#include <string>
#include <stdlib.h>

#include "log.h"

template <typename T>
inline void byte_write(std::ostream &out, T const &value)
{
  out.write(reinterpret_cast<char const*>(&value), sizeof(T));
}

template <typename T>
inline T byte_read(std::istream &in)
{
  T value;
  in.read(reinterpret_cast<char*>(&value), sizeof(T));
  return value;
}

template <>
inline void byte_write<std::string>(std::ostream &out, std::string const &value)
{
  byte_write<unsigned int>(out, value.size());
  for (unsigned int i(0); i < value.size(); ++i)
    byte_write<char>(out, value[i]);
}

template <>
inline std::string byte_read(std::istream &in)
{
  std::string text;
  unsigned int length = byte_read<unsigned int>(in);
  if (in.eof())
    return "";
  for (unsigned int i(0); i < length && !in.eof(); ++i)
    text += byte_read<char>(in);
  return text;
}

template <typename T>
inline void byte_write_old(std::ostream &out, T const &value)
{
  out.write(reinterpret_cast<char const*>(&value), sizeof(T));
}

template <typename T>
inline T byte_read_old(std::istream &in)
{
  T value;
  in.read(reinterpret_cast<char*>(&value), sizeof(T));
  return value;
}

template <>
inline void byte_write_old<std::string>(std::ostream &out, std::string const &value)
{
  byte_write<unsigned int>(out, static_cast<unsigned int>(value.size()));
  for (unsigned int i(0); i < value.size(); ++i)
    byte_write<char>(out, value[i]);
}

template <>
inline std::string byte_read_old(std::istream &in)
{
  std::string text;
  unsigned int length = byte_read<unsigned int>(in);
  if (in.eof())
    return "";
  for (unsigned int i(0); i < length && !in.eof(); ++i)
    text += byte_read<char>(in);
  return text;
}

template <typename T>
inline void byte_write_vector(std::ostream &out, std::vector<T> const &v)
{
  byte_write<unsigned int>(out, v.size());
  for (unsigned int i(0); i < v.size(); ++i)
    byte_write<T>(out, v[i]);
}

template <typename T>
inline std::vector<T> byte_read_vector(std::istream &in)
{
  unsigned int v_size = byte_read<unsigned int>(in);
  std::vector<T> v;
  for (unsigned int i(0); i < v_size; ++i)
  {
    //log(i, EXTRA);
    T const value = byte_read<T>(in);
    v.push_back(value);
  }
  return v;
}

template <typename T>
inline void byte_write_vector_old(std::ostream &out, std::vector<T> const &v)
{
  byte_write<unsigned int>(out, v.size());
  for (unsigned int i(0); i < v.size(); ++i)
    byte_write<T>(out, v[i]);
}

template <typename T>
inline std::vector<T> byte_read_vector_old(std::istream &in)
{
  unsigned int v_size = byte_read<unsigned int>(in);
  std::vector<T> v;
  for (unsigned int i(0); i < v_size; ++i)
  {
    T const value = byte_read<T>(in);
    v.push_back(value);
  }
  return v;
}

#endif
