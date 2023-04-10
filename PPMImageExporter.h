#ifndef __PPM_IMAGEEXPORTER_H_
#define __PPM_IMAGEEXPORTER_H_

#include <stdexcept>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <glm/glm.hpp>

/**
 * @brief This class is used to export an image in the ASCII PPM format
 *
 */
class PPMImageExporter
{

public:
    PPMImageExporter()
    {
    }

    void PPMImageExporter::exportToPPM(int width, int height, vector<glm::vec3> imageColors)
    {
        ofstream fp;
        fp.open("output.ppm");
        // write the word P3
        fp << "P3\n";
        // write width and height
        fp << width << " " << height << "/n";
        // write factor
        fp << "255\n";

        // write r,g,b values for
        for (auto &color : imageColors)
        {
            fp << char(color.x) << char(color.y) << char(color.z) << endl;
        }

        // close file
        fp.close();
    }
};

#endif