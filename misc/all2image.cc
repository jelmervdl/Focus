#include "all2image.h"

#include <iostream>
#include "../image/image_package.h"

using namespace std;

void all2image(PamImage &image, string filename)
{
    image.save(filename);
    cerr << "image " << filename << " was saved";
}