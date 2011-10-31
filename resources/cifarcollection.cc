#include "CifarCollection.h"

#include "../misc/context.h"
#include "allocator/fast_allocator.h"
#include "converter/cifarimageconverter.h"

CifarCollection::CifarCollection(std::string const &name, std::string const &type, std::string const &dir_name)
:
    Collection(name, type, dir_name)
{
    load();
}

void CifarCollection::clear()
{
    d_collection.clear();
}

void CifarCollection::load()
{
    clear();
        
    Context context;
    context["dir_name"] = d_dir_name;
    
    FastAllocator fast_allocator(context);
    CifarImageConverter stream_to_dataelement(context);

    FastAllocator::id_vector ids(fast_allocator.get_ids());
    
    for (uint32_t i(0); i < ids.size(); ++i)
    {
        std::auto_ptr<std::istream> in_stream(fast_allocator.read(ids[i]));
        try {
            // one cifar image file consists of multiple images (2500?)
            // concatenated into one file. Just read on baby!
            while (in_stream->good())
            {
                DataElement *data_element = stream_to_dataelement(*in_stream);
                d_collection.push_back(data_element);
            }
        } catch(...) {
            log(std::string("error reading file: ") + ids[i], EXTRA);
        }
    }
}

bool CifarCollection::exists() const
{
    return false;
}