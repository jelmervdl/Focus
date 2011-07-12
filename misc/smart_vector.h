#ifndef __SMART_VECTOR__
#define __SMART_VECTOR__

#include <vector>
#include "smart_pointer.h"

template <typename T>
class SmartVector : public std::vector<smart_pointer<T> >
{
 public:    
};

#endif
