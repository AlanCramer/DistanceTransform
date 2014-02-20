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


void DistTransUtil::ComputeDistTrans(const AcImage &in, AcImage &out)
{
    AcImage g = in; // this initialization just gets the sizing right
    AcImage dt = in; // this initialization just gets the sizing right

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
                if (w < m)
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

    out = dt;
}
