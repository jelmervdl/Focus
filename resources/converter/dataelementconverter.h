#ifndef __DATAELEMENT_CONVERTER__
#define __DATAELEMENT_CONVERTER__

#include "converter.h"
#include "../../datastructs/dataelement.h"
#include <sstream>

class DataElementConverter : public Converter<DataElement>
{
 public:
    DataElementConverter(Context &context) : Converter<DataElement>(context) {}
    
    DataElement *operator()(std::istream &in)
    {
        DataElement *element_ptr = new DataElement();
        DataElement &element = *element_ptr;
        
        element.data = byte_read_vector<float>(in);
        uint32_t map_n = byte_read<uint32_t>(in);
        for (uint32_t i(0); i < map_n; ++i)
        {
            std::string name(byte_read<std::string>(in));
            std::string value(byte_read<std::string>(in));
            
            element.meta_data[name] = value;
        }
        return element_ptr;
    }
    
    std::istream *operator()(DataElement &in)
    {
        std::stringstream *ioss_ptr = new std::stringstream();
        std::stringstream &ioss = *ioss_ptr;
        
        byte_write_vector(ioss, in.data);
        byte_write<uint32_t>(ioss, in.meta_data.size());
        Context::const_iterator it(in.meta_data.begin()), end(in.meta_data.end());
        
        for (; it != end; ++it)
        {
            byte_write(ioss, it->first);
            byte_write(ioss, it->second);
        }
        
        return ioss_ptr;
    }
    
};

#endif
