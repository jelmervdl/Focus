#ifndef __RESOURCE_ALLOCATOR__
#define __RESOURCE_ALLOCATOR__

#include "../../misc/misc_package.h"
#include <string>
#include <vector>
#include <iostream>
#include <memory>

class Allocator
{
 public:
    typedef std::string Id;
    typedef std::vector<Id> id_vector;
    
 public:
    Allocator(Context &context){}
    
    virtual Id new_id(){return Id();}
    
    virtual Id write(std::istream &data)
    {
        return write(data, new_id());
    }
    
    virtual Id write(std::istream &data, Id id){throw NotSupportedError();}
    
    virtual std::istream *read(Id id){throw NotSupportedError();}
    
    virtual id_vector get_ids(){throw NotSupportedError();}

    virtual void remove(Id id){throw NotSupportedError();}
    virtual void remove_all()
    {
        std::vector<Id> ids(get_ids());
        for (uint32_t i(0); i < ids.size(); ++i)
            remove(ids[i]);
    }
    
};

#endif
