#ifndef __LEARNABLE__
#define __LEARNABLE__

#include "../misc/log.h"

#include "dataelement.h"
#include <vector>
#include <cstring>

class Learnable : public DataElement
{
    typedef DataElement::data_vector data_vector;
    typedef std::string class_type;
    
 public:
    Learnable(data_vector const &data_vec, class_type const &a_class = "0")
        {
            data = data_vec;
            set_class(a_class);
            meta_data["uid"] = t_to_string(rand());
        }

	class_type const &get_class()
    {
        return meta_data["class"];
    }
    
    class_type const &set_class(class_type a_class)
    {
        return meta_data["class"] = a_class;
    }
    
    data_vector const &get_vector()
    {
        return data;
    }
    
    void set_attribute(std::string const &name, std::string const &value)
    {
        meta_data[name] = value;
    }
    
    std::string &operator()(std::string const &name)
    {
        return meta_data[name];
    }
    
    uint32_t uid()
    {
        return string_to_t<uint32_t>(meta_data["uid"]);
    }
};

#endif
