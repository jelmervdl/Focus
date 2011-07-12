#ifndef __CONVERTER__
#define __CONVERTER__

#include "../../misc/misc_package.h"

#include <iostream>

template <typename T>
class Converter
{
 public:
    Converter(Context &context){}
    
    virtual T *operator()(std::istream &in) = 0;
    virtual std::istream *operator()(T &in) = 0;
};

template <typename IN, typename OUT>
class UniConverter
{
 public:
    virtual OUT *operator()(IN &in) = 0;
};

#endif
