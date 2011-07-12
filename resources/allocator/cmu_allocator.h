#ifndef __CMU_ALLOCATOR__
#define __CMU_ALLOCATOR__

#include "allocator.h"

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <iterator>

class CMUAllocator : public Allocator
{
    std::string d_dir_name;
    std::vector<std::string> d_image_files;
    uint32_t d_i;
    
    void initialize(Context &context)
    {
        std::cout << "initialize" << std::endl;
        d_dir_name = context["dir_name"];
        
        std::vector<std::string> dirs = ls(d_dir_name);
        for (uint32_t i(0); i < dirs.size(); ++i)
        {
            std::string dir_name = d_dir_name + dirs[i];
            std::vector<std::string> sub_dir_files = ls(dir_name);
            for (uint32_t j(0); j < sub_dir_files.size(); ++j)
            {
                std::string file_name = dir_name + "/" + sub_dir_files[j];
                d_image_files.push_back(file_name);
            }   
            
        }
        d_i = 0;
    }
    
public:
    
    CMUAllocator(Context &context) : Allocator(context) {
        initialize(context);
    }
    
    virtual std::istream *read(Id id)
    {
        std::string space(" ");
        std::string filename(id);
        std::ifstream file_stream(filename.c_str());
        
        std::stringstream *ss_ptr = new std::stringstream();
        std::stringstream &ss = *ss_ptr;
        
        std::istringstream file_ss(space + filename);
        
        copy_stream(file_stream, ss);
        copy_stream(file_ss, ss);
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
