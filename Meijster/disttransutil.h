#ifndef DISTTRANSUTIL_H
#define DISTTRANSUTIL_H

#include "acimage.h"

class DistTransUtil
{
public:
    static void ComputeDistTrans(const AcImage& in, AcImage& out);
};

#endif // DISTTRANSUTIL_H
