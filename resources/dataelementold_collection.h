#ifndef __DATAELEMENT_OLD_COLLECTION__
#define __DATAELEMENT_OLD_COLLECTION__

#include "collection.h"

class DataElementOldCollection : public Collection
{   
    
 public:
    DataElementOldCollection(std::string name, std::string type, std::string dir_name)
    :
        Collection(name, type, dir_name)
    {
        log("creating data element collection", EXTRA);
        load();
    }

    void clear()
    {
        d_collection.clear();
    }

    void load()
    {
        log("loading:", EXTRA);
        clear();
        
        Context context;
        context["dir_name"] = d_dir_name;
        
        FastAllocator fast_allocator(context);
        DataElementOldConverter element_converter(context);

        log("before fast get ids", EXTRA);
        FastAllocator::id_vector ids(fast_allocator.get_ids());
        log(ids.size(), EXTRA);
        
        for (uint32_t i(0); i < ids.size(); ++i)
        {
            if (i % 100 == 0)
                log(i, EXTRA);
            std::auto_ptr<std::istream> in_stream(fast_allocator.read(ids[i]));
            
            if (! *in_stream)
                continue;
            try {
                DataElement *data_element = element_converter(*in_stream);
                
                d_collection.push_back(data_element);
                //data_element->meta_data.print();
            } catch(...) {}
        }
        log("done loading", EXTRA);
    }

    bool exists()
    {
        return false;
    }
    
    void save()
    {
        log("save func", EXTRA);
        Context context;
        context["dir_name"] = d_dir_name;
        
        FastAllocator fast_allocator(context);
        DataElementOldConverter element_converter(context);
        
        fast_allocator.remove_all();
        
        for (uint32_t i(0); i < d_collection.size(); ++i)
        {
            log("saving", EXTRA);
            FastAllocator::Id id = fast_allocator.new_id();
            std::auto_ptr<std::istream> stream(element_converter(*d_collection[i]));
            fast_allocator.write(*stream, id);
        }
    }
    
 private:
    void clear_directory()
    {
        Context context;
        context["dir_name"] = d_dir_name;
        
        FastAllocator fast_allocator(context);
        
        FastAllocator::id_vector ids(fast_allocator.get_ids());
        
        for (uint32_t i(0); i < ids.size(); ++i)
            fast_allocator.remove(ids[i]);
    }
};

#endif
