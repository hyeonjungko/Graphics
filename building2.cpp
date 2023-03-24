//! [code]

#include "Box.h"
#define GLM_FORCE_SWIZZLE
#include "BoxSet.h"
#include <fstream>
using namespace std;
#include "Material.h"

int main(int argc, char *argv[])
{
    BoxSet bset;
    ofstream outStream("scenegraphmodels/building2.txt");
    util::Material mat;
    mat.setAmbient(1, 1, 1);
    mat.setDiffuse(1, 1, 1);
    mat.setSpecular(1, 1, 1);
    mat.setShininess(10);

    // main box
    bset.add(Box(0, 0, 0, 10, 65, 10));
    bset.add(Box(0, 0, 10, 10, 50, 10));
    bset.add(Box(10, 0, 10, 10, 30, 10));
    bset.add(Box(10, 0, 0, 10, 10, 10));

    // add windows
    for (int i = 0; i < 5; i++)
    {
        bset.difference(Box(2, 5 + 10 * i, 18, 3, 3, 5));
        bset.add(Box(6, 5 + 10 * i, 20, 3, 3, 3));
    }

    bset.difference(Box(14, 0, 14, 6, 8, 6));

    // write it out to the above file. Using the one-argument version of
    // this function means it will export the scene graph with each box
    // of a different random color. When we incorporate lighting into the
    // scene you should regenerate your model using a specific material,
    // by using the two-argument version of this function
    bset.toScenegraph(outStream);
}

//! [code]
