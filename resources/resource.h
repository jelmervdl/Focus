#ifndef __RESOURCE__
#define __RESOURCE__

#include <iostream>
#include <string>
#include <memory>

#include "resources_package.h"
#include "../misc/misc_package.h"
#include "allocator/allocator_package.h"
#include "converter/converter_package.h"

/*
class Resource
{
 public:
    typedef AutoVector<PamImage> element_vector;
    typedef smart_pointer<element_vector> result_vector_sp;
    
 private:
    Allocator &d_allocator;
    Converter &d_converter;
    
 public:
    Resource(Allocator *allocator, Converter *converter)
    :
      d_allocator(*allocator),
      d_converter(*converter)
    {}
    
	virtual void write_all(element_vector &in)
    {
        log(in.size(), EXTRA);
        for (uint32_t i(0); i < in.size(); ++i)
        {
            log(i, EXTRA);
            try {
                std::auto_ptr<std::istream> a_stream(d_converter(*(in[i])));
                d_allocator.write(*a_stream); //TODO testen of dit werkt
            } catch(...) {}
        }
    }
    
    virtual result_vector_sp read_all()
    {
        element_vector *elements_ptr = new element_vector();
        element_vector &elements(*elements_ptr);
        
        std::vector<typename t_allocator::Id> ids(d_allocator.get_ids());
        
        for (uint32_t i(0); i < ids.size(); ++i)
        {
            std::auto_ptr<std::istream> in_stream(d_allocator.read(ids[i]));
            try {
                //log(ids[i], EXTRA);
                PamImage *element = d_converter(*in_stream);
                elements.push_back(element);
            } catch(...) {}
        }
        return smart_pointer<element_vector>(elements_ptr);
    } //read with auto ptr
};
 */
#endif
