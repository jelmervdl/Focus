#ifndef __PAMIMAGE_CONVERTER__
#define __PAMIMAGE_CONVERTER__

#include "converter.h"
#include "../../misc/misc_package.h"

class PamImageConverter : public Converter<PamImage>
{
 public:
    PamImageConverter(Context &context) : Converter<PamImage>(context){}
    
    PamImage *operator()(std::istream &in)
    {
        log("converting to pamimage");

        PamImage *image = new PamImage(in);
        log("done converting", 1);
        return image;
    }
    
    std::istream *operator()(PamImage &in)
    {
        log("converting to istream");
        std::string filename = get_temporary_filename();
        in.save(filename);
        return new std::ifstream(filename.c_str());
    }
};

#endif