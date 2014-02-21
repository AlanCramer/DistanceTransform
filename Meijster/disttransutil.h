#ifndef DISTTRANSUTIL_H
#define DISTTRANSUTIL_H

#include <map>
#include "acimage.h"


class DistTransUtil
{
public:

    // this is a singleton (hmmm- bad for parallelization)
    // to hold onto the directionMap
    DistTransUtil() ;

    // each pixel in 'out' will be the Euclidean distance squared to the nearest 0 in 'in'
    static void ComputeDistTrans(const AcImage& in, AcImage& dt);


    enum Direction{
        North,
        East,
        South,
        West,
        Corner,
        Stop
    };
    typedef std::map<uint32_t, Direction> DirectionMap;

    enum NeighborhoodValues
    {
        Interior, // 0, black, On (yep, black is on)
        Exterior,
        Boundary // meaning this neighbor is out of bounds
    };



    void VectorizeImage(const AcImage* in, AcImage& out);

private:
    void initDirectionMap();
    DirectionMap m_directionMap;
};

#endif // DISTTRANSUTIL_H
