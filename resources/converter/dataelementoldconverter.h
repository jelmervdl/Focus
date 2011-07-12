#ifndef __DATAELEMENT_OLD_CONVERTER__
#define __DATAELEMENT_OLD_CONVERTER__

#include "converter.h"
#include "../../datastructs/datastructs_package.h"
#include <sstream>

class DataElementOldConverter : public Converter<DataElement>
{
 public:
    DataElementOldConverter(Context &context) : Converter<DataElement>(context) {}
    
    DataElement *operator()(std::istream &in)
    {
        DataElement *element_ptr = new DataElement();
        DataElement &element = *element_ptr;
        
        element.data = byte_read_vector_old<float>(in);
        uint32_t map_n = byte_read<uint32_t>(in);
        for (uint32_t i(0); i < map_n; ++i)
        {
            std::string name(byte_read_old<std::string>(in));
            std::string value(byte_read_old<std::string>(in));
            element.meta_data[name] = value;
        }
        return element_ptr;
    }
    
    std::istream *operator()(DataElement &in)
    {
        std::stringstream *ioss_ptr = new std::stringstream();
        std::stringstream &ioss = *ioss_ptr;
        
        byte_write_vector_old(ioss, in.data);
        byte_write<uint32_t>(ioss, in.meta_data.size());
        Context::const_iterator it(in.meta_data.begin()), end(in.meta_data.end());
        
        for (; it != end; ++it)
        {
            byte_write_old(ioss, it->first);
            byte_write_old(ioss, it->second);
        }
        
        return ioss_ptr;
    }
    
};

#endif
