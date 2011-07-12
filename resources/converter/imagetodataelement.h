#ifndef __IMAGE_TO_DATAELEMENT__
#define __IMAGE_TO_DATAELEMENT__

#include "../../misc/misc_package.h"
#include "../../datastructs/datastructs_package.h"

class ImageToDataElement : public UniConverter<PamImage, DataElement>
{
 public:

    DataElement *operator()(PamImage &in)
    {
        DataElement *dataelement_ptr = new DataElement;
        DataElement &dataelement = *dataelement_ptr;
        
        dataelement.data = in.to_vector();
        
        std::string filename = in.getFileName();
        dataelement.meta_data["filename"] = filename;
        dataelement.meta_data["width"] = t_to_string(in.getWidth());
        dataelement.meta_data["height"] = t_to_string(in.getHeight());
        dataelement.meta_data["n_pixels"] = t_to_string(in.getHeight() * in.getWidth());
        dataelement.meta_data["format"] = in.getFormat();
        
        //if (filename.find("sunglasses") != std::string::npos)
        //    dataelement.set_attribute("eyes","sunglasses");
        //if (filename.find("open") != std::string::npos)
        //    dataelement.set_attribute("eyes","open");
        //if (filename.find("sad") != std::string::npos)
        //    dataelement.set_attribute("emotion","sad");
        //if (filename.find("happy") != std::string::npos)
        //    dataelement.set_attribute("emotion","happy");
        //dataelement.set_attribute("minval", t_to_string(in.get_minval()));
        //dataelement.set_attribute("maxval", t_to_string(in.get_maxval()));
        return dataelement_ptr;
    }
};

#endif
