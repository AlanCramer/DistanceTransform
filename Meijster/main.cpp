#include <iostream>
#include "lodepng.h"
#include "acimage.h"
#include "disttransutil.h"

using namespace std;


int main(int argc, char** argv)
{
    cout << "Hello World!" << endl;
    cout << " argc: " << argc << endl;

    for (int i = 0; i < argc; ++i)
    {
        cout << " argv[" << i << "] : "<< argv[i] << endl;
    }

    char* filename;
    if (argc == 2)
    {
        filename = argv[1];
    }
    else
    {
        cout << "Usage: Meister <filename>";
        return 0;
    }

    //DistTransUtil utils;
    AcImage acimage;
    acimage.loadImage(filename);
    AcImage trf = acimage;
    AcImage vectorized = acimage;


    cout << "Attempted read of " <<  filename << endl;
    cout << "  " << acimage.getLoadStatusText() << endl;

    if (acimage.loadSuccess())
    {
        acimage.debugDump();

        DistTransUtil::ComputeDistTrans(acimage, trf);

        trf.debugDump();

        DistTransUtil utils;
        utils.VectorizeDistanceTrf(trf, vectorized);
    }

    return 0;
}

