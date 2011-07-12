#ifndef __FAST_ALLOCATOR__
#define __FAST_ALLOCATOR__

#include "allocator.h"

#include <fstream>
#include <string>
#include <sstream>

class FastAllocator : public Allocator
{
    uint32_t d_i;
    std::string d_preamble;
    std::string d_dir_name;
    
    void initialize(Context &context)
    {
        d_i = 0;
        d_preamble = "object_";
        d_dir_name = context["dir_name"];
        d_i = ls(d_dir_name).size();
    }

public:    
    
    FastAllocator(Context &context) : Allocator(context) {
        initialize(context); 
    }

    Id new_id()
    {
        Id id = d_preamble + t_to_string<>(d_i);
        ++d_i;
        return id;
    }
    
    virtual Id write(std::istream &data, Id id)
    {
        std::string name = d_dir_name + id;
        std::ofstream out(name.c_str());
        copy_stream(data, out);
        return id;
    }
    
    std::istream *read(Id id)
    {
        std::string in_name = d_dir_name + t_to_string<>(id);
        return new std::ifstream(in_name.c_str());
    }
    
    
    std::vector<Id> get_ids()
    {
        std::vector<Id> ids(ls(d_dir_name));
        std::vector<Id> new_ids;
        
        for (uint32_t i(0); i < ids.size(); ++i)
        {
            if (ids[i].size() > 0 && ids[i][0] != '.' )
                new_ids.push_back(ids[i]);
        }
        return new_ids;
    }

    void remove(Id id)
    {
        std::string file_name = d_dir_name + t_to_string<>(id);
        remove_file(file_name);
    }
    
    void remove_all()
    {
        std::vector<Id> ids(get_ids());
        for (uint32_t i(0); i < ids.size(); ++i)
            remove(ids[i]);
        d_i = 0;
    }
};

#endif
