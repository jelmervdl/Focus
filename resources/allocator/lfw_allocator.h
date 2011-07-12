#ifndef __LFW_ALLOCATOR__
#define __LFW_ALLOCATOR__

#include "allocator.h"

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>

class LFWAllocator : public Allocator
{
    std::string d_dir_name;
    std::vector<std::string> d_image_files;
    uint32_t d_i;
    
    void initialize(Context &context)
    {
        std::cout << "initialize" << std::endl;
        d_dir_name = context["dir_name"];
        d_image_files = ls(d_dir_name);
        d_i = 0;
    }
public:
    
    LFWAllocator(Context &context) : Allocator(context) {
        initialize(context);
    }
    
    virtual std::istream *read(Id id)
    {
        std::string filename(d_dir_name + id);
        std::ifstream file_stream(filename.c_str());
        
        std::stringstream *ss_ptr = new std::stringstream();
        std::stringstream &ss = *ss_ptr;
        
        copy_stream(file_stream, ss);
        byte_write<>(ss, filename);
        //log(ss.str(), EXTRA);
        return ss_ptr;
    }
    
    virtual std::vector<Id> get_ids()
    {
        return d_image_files;
    }
    
    bool empty()
    {
        std::cout << "bloe " << d_dir_name << " " << d_image_files.size() << std::endl;
        return d_i >= d_image_files.size();
    }
};

#endif
