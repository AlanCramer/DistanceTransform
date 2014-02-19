#ifndef ACIMAGE_H
#define ACIMAGE_H

#include <vector>
#include "lodepng.h"

const unsigned int AcBitDepth = 16;


class AcImage
{
public:
    AcImage();

    bool loadImage(const char* filename);


    void debugDump() const;

    unsigned int getImageAt(unsigned int x, unsigned int y) { return m_pixels[m_width * x + y]; }

    void setWidth(unsigned int width) { m_width = width; }
    unsigned int getWidth() const { return m_width; }

    void setHeight(unsigned int height) { m_height = height; }
    unsigned int getHeight() const { return m_height; }

    bool loadSuccess() const { return m_loadStatus == 0; }
    const char* getLoadStatusText() const { return lodepng_error_text(m_loadStatus); }

 private:
    std::vector<unsigned char> m_pixels; //the raw pixels
    unsigned int m_width;
    unsigned int m_height;

    const char* m_filename;
    unsigned int m_loadStatus;
};

#endif // ACIMAGE_H
