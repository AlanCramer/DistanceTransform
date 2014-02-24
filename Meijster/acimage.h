#ifndef ACIMAGE_H
#define ACIMAGE_H

#include <vector>
#include "lodepng.h"

const unsigned int AcBitDepth = 16;
typedef unsigned char AcBitDepthType; // problem with lodepng, higher bitdepth, I'm hoping for more resolution

class AcImage
{
public:
    static const unsigned int MaxImageVal = (2^AcBitDepth) -1;

    AcImage();

    bool loadImage(const char* filename);

    void debugDump() const;

    // turn everything to zero except the value
    void maskImage(AcBitDepthType value);

    // turn everything to zero except those values less than or equal, which all become value
    void thresholdImage(AcBitDepthType value);


    void getNbrhd(unsigned int x, unsigned int y, uint8_t* nw, uint8_t* nn, uint8_t* ne, uint8_t* ww, uint8_t* cc, uint8_t* ee, uint8_t* sw, uint8_t* ss, uint8_t* se);

    AcBitDepthType& getImageAt(unsigned int x, unsigned int y) { return m_pixels[m_width * y + x]; }
    const AcBitDepthType& getImageAt(unsigned int x, unsigned int y) const { return m_pixels[m_width * y + x]; }

    void setWidth(unsigned int width) { m_width = width; }
    unsigned int getWidth() const { return m_width; }

    void setHeight(unsigned int height) { m_height = height; }
    unsigned int getHeight() const { return m_height; }

    bool loadSuccess() const { return m_loadStatus == 0; }
    const char* getLoadStatusText() const { return lodepng_error_text(m_loadStatus); }

    AcBitDepthType& operator()(unsigned int x, unsigned int y) { return getImageAt(x, y); }
    const AcBitDepthType& operator()(unsigned int x, unsigned int y) const { return getImageAt(x, y); }

 private:
    std::vector<AcBitDepthType> m_pixels; //the raw pixels
    unsigned int m_width;
    unsigned int m_height;

//    const char* m_filename;
    unsigned int m_loadStatus;
};

#endif // ACIMAGE_H
