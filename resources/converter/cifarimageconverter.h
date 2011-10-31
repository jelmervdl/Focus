#ifndef __CIFAR_IMAGE_CONVERTER__
#define __CIFAR_IMAGE_CONVERTER__

#include "converter.h"
#include "../../datastructs/dataelement.h"
#include <sstream>

class CifarImageConverter : public Converter<DataElement>
{
 public:
    CifarImageConverter(Context &context);
    
    DataElement *operator()(std::istream &in);
    
    std::istream *operator()(DataElement &in);
};

#endif
