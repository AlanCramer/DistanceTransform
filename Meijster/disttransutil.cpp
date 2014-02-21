#include "disttransutil.h"
#include "iostream"


namespace {

// g(i) = G(i, y) for some fixed y
// Sep(i, u) = (u^2 - i^2 +g(u)^2-g(i)^2) div (2(u-i))  where div is integer division
int Sep(const AcImage& G, unsigned int y, int i, int u)
{
    return (u*u - i*i + G(u,y)*G(u,y) - G(i, y)*G(i, y)) / (2*(u-i));
}

// f using EDT is: f(x, i) = (x-i)^2 + g(i)^2
int EDT_f(const AcImage& G, unsigned int y, int x, int i)
{
    return (x-i)*(x-i) + G(i,y)*G(i,y);
}
}


DistTransUtil::DistTransUtil()
{
    initDirectionMap();
}

void DistTransUtil::ComputeDistTrans(const AcImage &in, AcImage &dt)
{
    AcImage g = in; // this initialization just gets the sizing right

    unsigned int m = in.getWidth();
//    unsigned int n = in.getHeight();

    const unsigned int maxGVal = in.getWidth() + in.getHeight();

    // phase 1 - define g(i,j)
    //  todo - this seems wrong if w or h is pretty large.
    //         distances can get too large to store in AcImage's unsigned chars.

    //  todo - last column is uninitialized, what should it be?

    for (unsigned int i = 0; i < m; ++i)
    {
        g(i, 0) = (in(i, 0) == 0) ? 0 : maxGVal;

        for (unsigned int j = 1; j < in.getHeight(); ++j)
        {
            g(i,j) = (in(i,j) == 0) ? 0 : 1 + g(i, j-1);
        }

        for (int j = in.getHeight() - 1; j >= 0; --j)
        {
            if (g(i, j+1) < g(i,j))
            {
                g(i,j) = 1 + g(i, j+1);
            }
        }
    }

    std::cout << "\n\nG(x,y) :\n" << std::endl;
    g.debugDump();
    std::cout << "\n\n" << std::endl;

    // Phase 2
    // need f and Sep (defined above)

    // todo, this loop could be parallelized
    for (unsigned int j = 0; j < in.getHeight(); ++j)
    {
        int q = 0;
        int s[m];
        int t[m];
        s[0] = t[0] = 0;

        for (unsigned int u = 1; u < m; ++u)
        {
            while (q >=0 && EDT_f(g, j, t[q], s[q]) > EDT_f(g, j, t[q], u))
            {
                --q;
            }

            if (q < 0)
            {
                q = 0;
                s[0] = u;
            }
            else
            {
                int w = 1 + Sep(g, j, s[q], u);
                if (w < 0 || (unsigned) w < m) // is w <0 possible?
                {
                    ++q;
                    s[q] = u;
                    t[q] = w;
                }
            }
        }

        for (int u = m; u >=0; --u)
        {
            dt(u, j) = EDT_f(g, j, u, s[q]);
            if (u == t[q])
            {
                --q;
            }
        }
    }

    std::cout << "\nWidth, Height = " << dt.getWidth() << " " << dt.getHeight()<< std::endl;
}

// for the vectorization
namespace
{
    uint32_t encodeNbrhd(uint8_t nw, uint8_t nn, uint8_t ne,
                                uint8_t ww, uint8_t cc, uint8_t ee,
                                uint8_t sw, uint8_t ss, uint8_t se)
    {
        return  (nw <<  0) + (nn <<  2) + (ne <<  4) +
                (ww <<  6) + (cc <<  8) + (ee << 10) +
                (sw << 12) + (ss << 14) + (se << 16);
    }

    uint32_t encodeNbrhdRot90(uint8_t nw, uint8_t nn, uint8_t ne,
                                uint8_t ww, uint8_t cc, uint8_t ee,
                                uint8_t sw, uint8_t ss, uint8_t se)
    {
        return  (sw <<  0) + (ww <<  2) + (nw <<  4) +
                (ss <<  6) + (cc <<  8) + (nn << 10) +
                (se << 12) + (ee << 14) + (ne << 16);
    }

    uint32_t encodeNbrhdRot180(uint8_t nw, uint8_t nn, uint8_t ne,
                                uint8_t ww, uint8_t cc, uint8_t ee,
                                uint8_t sw, uint8_t ss, uint8_t se)
    {
        return  (se <<  0) + (ss <<  2) + (sw <<  4) +
                (ee <<  6) + (cc <<  8) + (ww << 10) +
                (ne << 12) + (nn << 14) + (nw << 16);
    }

    uint32_t encodeNbrhdRot270(uint8_t nw, uint8_t nn, uint8_t ne,
                                uint8_t ww, uint8_t cc, uint8_t ee,
                                uint8_t sw, uint8_t ss, uint8_t se)
    {
        return  (ne <<  0) + (ee <<  2) + (se <<  4) +
                (nn <<  6) + (cc <<  8) + (ss << 10) +
                (nw << 12) + (ww << 14) + (sw << 16);
    }

    DistTransUtil::Direction RotDir90(DistTransUtil::Direction dir)
    {
        if (dir == DistTransUtil::East)
            return DistTransUtil::South;

        if (dir == DistTransUtil::South)
            return DistTransUtil::West;

        if (dir == DistTransUtil::West)
            return DistTransUtil::North;

        if (dir == DistTransUtil::North)
            return DistTransUtil::East;

        // corner and stop maps to themselves
        return dir;
    }


    DistTransUtil::Direction RotDir180(DistTransUtil::Direction dir)
    {
        if (dir == DistTransUtil::East)
            return DistTransUtil::West;

        if (dir == DistTransUtil::South)
            return DistTransUtil::North;

        if (dir == DistTransUtil::West)
            return DistTransUtil::East;

        if (dir == DistTransUtil::North)
            return DistTransUtil::South;

        // corner and stop maps to themselves
        return dir;
    }

    DistTransUtil::Direction RotDir270(DistTransUtil::Direction dir)
    {
        if (dir == DistTransUtil::East)
            return DistTransUtil::North;

        if (dir == DistTransUtil::South)
            return DistTransUtil::East;

        if (dir == DistTransUtil::West)
            return DistTransUtil::South;

        if (dir == DistTransUtil::North)
            return DistTransUtil::West;

        // corner and stop maps to themselves
        return dir;
    }


    void addDirMapEntry(DistTransUtil::DirectionMap& dirMap,
                        uint8_t nw, uint8_t nn, uint8_t ne,
                        uint8_t ww, uint8_t cc, uint8_t ee,
                        uint8_t sw, uint8_t ss, uint8_t se,
                        DistTransUtil::Direction dir)
    {
        dirMap[encodeNbrhd(nw, nn, ne, ww, cc, ee, sw, ss, se)] = dir;
        dirMap[encodeNbrhdRot90(nw, nn, ne, ww, cc, ee, sw, ss, se)] = RotDir90(dir);
        dirMap[encodeNbrhdRot180(nw, nn, ne, ww, cc, ee, sw, ss, se)] = RotDir180(dir);
        dirMap[encodeNbrhdRot270(nw, nn, ne, ww, cc, ee, sw, ss, se)] = RotDir270(dir);
    }
}

void DistTransUtil::initDirectionMap()
{
    // 0 = empty
    // 1 = interior
    // 2 = boundary
    //
    // 011
    // 111
    // 111
    addDirMapEntry(m_directionMap, 0,1,1,1,1,1,1,1,1, DistTransUtil::North);
    // 101
    // 111
    // 111
    addDirMapEntry(m_directionMap, 1,0,1,1,1,1,1,1,1, DistTransUtil::East);
    //
    // 2 0's:
    //
    // 001
    // 111
    // 111
    addDirMapEntry(m_directionMap, 0,0,1,1,1,1,1,1,1,DistTransUtil::East);
    // 100
    // 111
    // 111
    addDirMapEntry(m_directionMap, 1,0,0,1,1,1,1,1,1,DistTransUtil::East);
    // 010
    // 111
    // 111
    addDirMapEntry(m_directionMap, 0,1,0,1,1,1,1,1,1,DistTransUtil::East);
    // 011
    // 110
    // 111
    addDirMapEntry(m_directionMap, 0,1,1,1,1,0,1,1,1,DistTransUtil::South);
    // 110
    // 011
    // 111
    addDirMapEntry(m_directionMap,1,1,0,0,1,1,1,1,1,DistTransUtil::East);
    // 101
    // 011
    // 111
    addDirMapEntry(m_directionMap,1,0,1,0,1,1,1,1,1,DistTransUtil::East);
    // 101
    // 110
    // 111
    addDirMapEntry(m_directionMap,1,0,1,1,1,0,1,1,1,DistTransUtil::South);
    // 011
    // 111
    // 110
    addDirMapEntry(m_directionMap,0,1,1,1,1,1,1,1,0,DistTransUtil::Corner);
    // 011
    // 111
    // 101
    addDirMapEntry(m_directionMap,0,1,1,1,1,1,1,0,1,DistTransUtil::North);
    // 110
    // 111
    // 101
    addDirMapEntry(m_directionMap,1,1,0,1,1,1,1,0,1,DistTransUtil::West);
    // 101
    // 111
    // 110
    addDirMapEntry(m_directionMap,1,0,1,1,1,1,1,1,0,DistTransUtil::South);
    // 101
    // 111
    // 011
    addDirMapEntry(m_directionMap,1,0,1,1,1,1,0,1,1,DistTransUtil::East);
    //
    // 3 0's:
    //
    // 001
    // 011
    // 111
    addDirMapEntry(m_directionMap,0,0,1,0,1,1,1,1,1,DistTransUtil::East);
    // 010
    // 011
    // 111
    addDirMapEntry(m_directionMap,0,1,0,0,1,1,1,1,1,DistTransUtil::East);
    // 010
    // 110
    // 111
    addDirMapEntry(m_directionMap,0,1,0,1,1,0,1,1,1,DistTransUtil::South);
    // 010
    // 111
    // 011
    addDirMapEntry(m_directionMap,0,1,0,1,1,1,0,1,1,DistTransUtil::East);
    // 010
    // 111
    // 110
    addDirMapEntry(m_directionMap,0,1,0,1,1,1,1,1,0,DistTransUtil::South);
    // 110
    // 011
    // 011
    addDirMapEntry(m_directionMap,1,1,0,0,1,1,0,1,1,DistTransUtil::East);
    // 011
    // 110
    // 110
    addDirMapEntry(m_directionMap,0,1,1,1,1,0,1,1,0,DistTransUtil::South);
    // 101
    // 011
    // 011
    addDirMapEntry(m_directionMap,1,0,1,0,1,1,0,1,1,DistTransUtil::East);
    // 101
    // 110
    // 110
    addDirMapEntry(m_directionMap,1,0,1,1,1,0,1,1,0,DistTransUtil::South);
    // 011
    // 011
    // 011;
    addDirMapEntry(m_directionMap,0,1,1,0,1,1,0,1,1,DistTransUtil::North);
    //
    // 4 0's:
    //
    // 001
    // 011
    // 011
    addDirMapEntry(m_directionMap,0,0,1,0,1,1,0,1,1,DistTransUtil::East);
    // 100
    // 110
    // 110
    addDirMapEntry(m_directionMap,1,0,0,1,1,0,1,1,0,DistTransUtil::South);
    // 010
    // 011
    // 011
    addDirMapEntry(m_directionMap,0,1,0,0,1,1,0,1,1,DistTransUtil::East);
    // 010
    // 110
    // 110
    addDirMapEntry(m_directionMap,0,1,0,1,1,0,1,1,0,DistTransUtil::South);
    // 001
    // 110
    // 110
    addDirMapEntry(m_directionMap,0,0,1,1,1,0,1,1,0,DistTransUtil::South);
    // 100
    // 011
    // 011
    addDirMapEntry(m_directionMap,1,0,0,0,1,1,0,1,1,DistTransUtil::East);
    //
    // 5 0's:
    //
    // 000
    // 011
    // 011
    addDirMapEntry(m_directionMap,0,0,0,0,1,1,0,1,1,DistTransUtil::East);
    //
    // boundary states
    //
    // 200
    // 211
    // 211
    addDirMapEntry(m_directionMap,2,0,0,2,1,1,2,1,1,DistTransUtil::East);
    // 201
    // 211
    // 211
    addDirMapEntry(m_directionMap,2,0,1,2,1,1,2,1,1,DistTransUtil::East);
    // 210
    // 211
    // 211
    addDirMapEntry(m_directionMap,2,1,0,2,1,1,2,1,1,DistTransUtil::East);
    // 002
    // 112
    // 112
    addDirMapEntry(m_directionMap,0,0,2,1,1,2,1,1,2,DistTransUtil::Stop);
    // 102
    // 112
    // 112
    addDirMapEntry(m_directionMap,1,0,2,1,1,2,1,1,2,DistTransUtil::Stop);
    // 002
    // 112
    // 102
    addDirMapEntry(m_directionMap,0,0,2,1,1,2,1,0,2,DistTransUtil::Stop);
    // 012
    // 112
    // 112
    addDirMapEntry(m_directionMap,0,1,2,1,1,2,1,1,2,DistTransUtil::Stop);
    // 012
    // 112
    // 102
    addDirMapEntry(m_directionMap,0,1,2,1,1,2,1,0,2,DistTransUtil::Stop);
}
