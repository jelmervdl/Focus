#ifndef __BINARY_PAMIMAGE_CONVERTER__
#define __BINARY_PAMIMAGE_CONVERTER__

#include "../../image/image_package.h"
#include "converter.h"
#include <sstream>

class BinaryPamImageConverter : public Converter<PamImage>
{
 public:
    BinaryPamImageConverter(Context &context) : Converter<PamImage>(context) {}
    
    PamImage *operator()(std::istream &in)
    {
        PamImage *image = new PamImage();
        log("before loading binary");
        image->load_from_binary_stream(in);
        log("after loading binary");
        return image;
    }
    
    std::istream *operator()(PamImage &in)
    {
        std::stringstream *ioss_ptr = new std::stringstream();
        std::stringstream &ioss = *ioss_ptr;
        in.save_to_binary_stream(ioss);
        return ioss_ptr;
    }
    
};

#endif