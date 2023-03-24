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
    ofstream outStream("scenegraphmodels/building1.txt");
    util::Material mat;
    mat.setAmbient(1, 1, 1);
    mat.setDiffuse(1, 1, 1);
    mat.setSpecular(1, 1, 1);
    mat.setShininess(10);

    // main box
    bset.add(Box(0, 0, 0, 35, 60, 35));

    // add second box
    bset.add(Box(55, 0, 0, 15, 55, 35));

    for (int i = 0; i < 5; i++)
    {
        // add floors
        bset.add(Box(0, 7 + 10 * i, 0, 70, 3, 35));
        // add pillars for each floor
        for (int j = 0; j < 3; j++)
        {
            bset.difference(Box(65, 10 * i, 10 * j, 5, 5, 5));
        }
    }

    // write it out to the above file. Using the one-argument version of
    // this function means it will export the scene graph with each box
    // of a different random color. When we incorporate lighting into the
    // scene you should regenerate your model using a specific material,
    // by using the two-argument version of this function
    bset.toScenegraph(outStream);
}

//! [code]
