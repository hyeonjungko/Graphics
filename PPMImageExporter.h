#ifndef __PPM_IMAGEEXPORTER_H_
#define __PPM_IMAGEEXPORTER_H_

#include <stdexcept>
#include <iostream>
#include <sstream>
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

    void PPMImageExporter::export(int width, int height, vector<glm::vec3> image)
    {
        ifstream fp;
        fp.open("output.ppm");
        // write the word P3
        fp << "P3\n";
        // write width and height
        fp << width << " " << height << "/n";
        // write factor
        fp << "255\n";
        for (auto &color : image)
        { // TODO: confirm this writes the proper PPM image
            fp << color.x;
            fp << color.y;
            fp << color.z;
            fp << "/n";
        }

        // close file
        fp.close();
    }
};

#endif