#ifndef __LFW_COLLECTION__
#define __LFW_COLLECTION__

#include "collection.h"

class LFWCollection : public Collection
{   
    
 public:
    LFWCollection(std::string name, std::string type, std::string dir_name)
    :
        Collection(name, type, dir_name)
    {
        load();
    }

    void clear()
    {
        d_collection.clear();
    }

    void load()
    {
        clear();
        
        Context context;
        context["dir_name"] = d_dir_name;
        
        FastAllocator fast_allocator(context);
        BinaryPamImageConverter bin_to_image(context);
        ImageToDataElement image_to_dataelement;

        FastAllocator::id_vector ids(fast_allocator.get_ids());
        
        for (uint32_t i(0); i < ids.size(); ++i)
        {
            if (i % 1000 == 0)
                log(i, EXTRA);
            std::auto_ptr<std::istream> in_stream(fast_allocator.read(ids[i]));
            try {
                //log(ids[i], EXTRA);
                PamImage *image = bin_to_image(*in_stream);
                DataElement *data_element = image_to_dataelement(*image);
                delete image;
                
                d_collection.push_back(data_element);
            } catch(...) {
                log(std::string("error reading file: ") + ids[i], EXTRA);
                
            }
        }
    }

    bool exists()
    {
        return false;
    }
};

#endif
