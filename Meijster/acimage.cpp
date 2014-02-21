#include "acimage.h"
#include "lodepng.h"

#include <iostream>

using namespace std;

namespace
{
void debugDumpImageData(const std::vector<unsigned char>& image, const unsigned int width, const unsigned int height)
{
    for (unsigned int i = 0; i < height; ++i)
    {
        for (unsigned int j = 0; j < width; ++j)
        {
            std::cout.width(3);
            cout << (unsigned int) image[i*width +j];
        }
        cout << endl;
    }
}
}

AcImage::AcImage()
{
}

// true if all okay
bool AcImage::loadImage(const char *filename)
{
//    m_filename = filename;
    unsigned int err = lodepng::decode(m_pixels, m_width, m_height, filename, LCT_GREY,8);

    m_loadStatus = err;

    return m_loadStatus == 0; // no error
}



void AcImage::debugDump() const
{
    debugDumpImageData(m_pixels, m_width, m_height);
}
