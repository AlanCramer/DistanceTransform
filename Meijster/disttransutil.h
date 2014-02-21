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
    static void ComputeDistTrans(const AcImage &in, AcImage& dt);

    static void debugDumpImageAsDir(const AcImage& img);

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

    Direction nbrhdToDir(uint8_t nw, uint8_t nn, uint8_t ne,
                         uint8_t ww, uint8_t cc, uint8_t ee,
                         uint8_t sw, uint8_t ss, uint8_t se);
    void VectorizeDistanceTrf(AcImage dt, AcImage& out);

private:
    void initDirectionMap();
    DirectionMap m_directionMap;
};

#endif // DISTTRANSUTIL_H
