#ifndef DISTTRANSUTIL_H
#define DISTTRANSUTIL_H

#include <iostream>
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

    void dumpDirMap(const char *filename);

private:
    void initDirectionMap();
    DirectionMap m_directionMap;
};


inline std::ostream& operator<<(std::ostream& out, const DistTransUtil::Direction value){
    const char* s = 0;
#define PROCESS_VAL(p) case(p): s = #p; break;
    switch(value){
        PROCESS_VAL(DistTransUtil::North);
        PROCESS_VAL(DistTransUtil::East);
        PROCESS_VAL(DistTransUtil::South);
        PROCESS_VAL(DistTransUtil::West);
        PROCESS_VAL(DistTransUtil::Corner);
        PROCESS_VAL(DistTransUtil::Stop);
    default:
            s= "unknown Direction value";
    }
#undef PROCESS_VAL

    return out << s;
}


#endif // DISTTRANSUTIL_H
