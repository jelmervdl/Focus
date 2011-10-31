#include "cifarimageconverter.h"

typedef struct
{
    char label;
    char r[32 * 32];
    char g[32 * 32];
    char b[32 * 32];

    std::vector<float> as_vector() const;

    std::vector<float> as_grayscale_vector() const;
} cifar_image;

typedef struct
{
    char label;
    char pixels[3 * 32 * 32];
} cifar_image_chunk;

typedef union
{
     cifar_image image;
     cifar_image_chunk stream;
} cifar_image_data;

/*
static_assert(sizeof(cifar_image) == 3073,
    "The CIFAR image struct is not equal to the one described in the specs");

static_assert(sizeof(cifar_image) == sizeof(cifar_image_stream),
    "The CIFAR image struct and the stream version of the struct differ in size");
*/

int grayscale_sum(int const &r, int const &g, int const &b)
{
    return r + g + b;
}

template<class T>
T maximum(T const values[], int length)
{
    T max = 0;
    for (T i = 0; i < length; ++i)
        if (values[i] > max)
            max = values[i];

    return max;
}

std::vector<float> cifar_image::as_vector() const
{
    cifar_image_data data;
    data.image = *this;

    std::vector<float> output(3 * 32 * 32);

    char r_max = maximum(r, 32 * 32),
        g_max = maximum(g, 32 * 32),
        b_max = maximum(b, 32 * 32),
        max = r_max;
    
    if (g_max > max)
        max = g_max;
    
    if (b_max > max)
        max = b_max;
    
    for (int i = 0; i < 3 * 32 * 32; ++i)
        output.push_back((float) data.stream.pixels[i] / max);
    
    return output;
}

std::vector<float> cifar_image::as_grayscale_vector() const
{
    std::vector<float> output(32 * 32);
    int grayscale_pixels[32 * 32];

    for (int i = 0; i < 32 * 32; ++i)
        grayscale_pixels[i] = grayscale_sum(r[i], g[i], b[i]);

    int max = maximum(grayscale_pixels, 32 * 32);

    assert(max > 0);

    for (int i = 0; i < 32 * 32; ++i)
        output.push_back((float) grayscale_pixels[i] / max);
    
    return output;
}

CifarImageConverter::CifarImageConverter(Context &context)
:
    Converter<DataElement>(context)
{}

DataElement *CifarImageConverter::operator()(std::istream &in)
{
    DataElement *element_ptr = new DataElement();
    DataElement &element = *element_ptr;
    
    cifar_image image(byte_read<cifar_image>(in));

    element.meta_data["label"] = image.label;
    element.data = image.as_grayscale_vector();

    return element_ptr;
}

std::istream *CifarImageConverter::operator()(DataElement &in)
{
    throw std::logic_error("CifarImageConverter::operator(DataElement &in) is not implemented");
}