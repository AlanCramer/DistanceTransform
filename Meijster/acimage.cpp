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



void AcImage::maskImage(AcBitDepthType value)
{
    for (unsigned int i = 0; i < m_width; ++i)
    {
        for (unsigned int j = 0; j < m_height; ++j)
        {
            if (getImageAt(i,j) != value)
                getImageAt(i,j) = 0;
        }
    }
}

void AcImage::thresholdImage(AcBitDepthType value)
{
    for (unsigned int i = 0; i < m_width; ++i)
    {
        for (unsigned int j = 0; j < m_height; ++j)
        {
            getImageAt(i,j) =(getImageAt(i,j) <= value)? value : 0;
        }
    }
}

// todo, this should return a 3x3 image
// todo, encodeImage should be on the AcImage
void AcImage::getNbrhd(unsigned int x, unsigned int y, uint8_t *nw, uint8_t *nn, uint8_t *ne, uint8_t *ww, uint8_t *cc, uint8_t *ee, uint8_t *sw, uint8_t *ss, uint8_t *se)
{
    if (x>0 && y>0 && x<m_width-1 && y<m_height-1)
    {
        *nw = getImageAt(x-1, y-1);
        *nn = getImageAt(x  , y-1);
        *ne = getImageAt(x+1, y-1);
        *ww = getImageAt(x-1, y  );
        *cc = getImageAt(x  , y  );
        *ee = getImageAt(x+1, y  );
        *sw = getImageAt(x-1, y+1);
        *ss = getImageAt(x  , y+1);
        *se = getImageAt(x+1, y+1);
    }
}
