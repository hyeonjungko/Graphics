#ifndef _SCENEGRAPHIMPORTER_H_
#define _SCENEGRAPHIMPORTER_H_

#include <ObjImporter.h>
#include "../ImageLoader.h"
#include "../PPMImageLoader.h"
#include "TextureImage.h"
#include "IScenegraph.h"
#include "Scenegraph.h"
#include "Light.h"
#include "GroupNode.h"
#include "LeafNode.h"
#include "TransformNode.h"
#include "RotateTransform.h"
#include "ScaleTransform.h"
#include "TranslateTransform.h"
// #include "PolygonMesh.h"
#include "Material.h"
#include <istream>
#include <map>
#include <string>
#include <iostream>
using namespace std;

namespace sgraph
{
    class ScenegraphImporter
    {
    public:
        ScenegraphImporter()
        {
        }

        IScenegraph *parse(istream &input)
        {
            string command;
            string inputWithOutCommentsString = stripComments(input);
            istringstream inputWithOutComments(inputWithOutCommentsString);
            while (inputWithOutComments >> command)
            {
                cout << "Read " << command << endl;
                if (command == "instance")
                {
                    string name, path;
                    inputWithOutComments >> name >> path;
                    cout << "Read " << name << " " << path << endl;
                    meshPaths[name] = path;
                    ifstream in(path);
                    if (in.is_open())
                    {
                        util::PolygonMesh<VertexAttrib> mesh = util::ObjImporter<VertexAttrib>::importFile(in, false);
                        meshes[name] = mesh;
                    }
                }
                else if (command == "texture-image")
                {
                    parseTextureImage(inputWithOutComments);
                }
                else if (command == "group")
                {
                    parseGroup(inputWithOutComments);
                }
                else if (command == "leaf")
                {
                    parseLeaf(inputWithOutComments);
                }
                else if (command == "material")
                {
                    parseMaterial(inputWithOutComments);
                }
                else if (command == "scale")
                {
                    parseScale(inputWithOutComments);
                }
                else if (command == "rotate")
                {
                    parseRotate(inputWithOutComments);
                }
                else if (command == "translate")
                {
                    parseTranslate(inputWithOutComments);
                }
                else if (command == "light")
                {
                    parseLight(inputWithOutComments);
                }
                else if (command == "attach-light")
                {
                    parseAttachLight(inputWithOutComments);
                }
                else if (command == "copy")
                {
                    parseCopy(inputWithOutComments);
                }
                else if (command == "import")
                {
                    parseImport(inputWithOutComments);
                }
                else if (command == "assign-material")
                {
                    parseAssignMaterial(inputWithOutComments);
                }
                else if (command == "assign-texture")
                {
                    parseAssignTexture(inputWithOutComments);
                }
                else if (command == "add-child")
                {
                    parseAddChild(inputWithOutComments);
                }
                else if (command == "assign-root")
                {
                    parseSetRoot(inputWithOutComments);
                }
                else
                {
                    throw runtime_error("Unrecognized or out-of-place command: " + command);
                }
            }
            if (root != NULL)
            {
                IScenegraph *scenegraph = new Scenegraph();
                scenegraph->makeScenegraph(root);
                scenegraph->setMeshes(meshes);
                scenegraph->setMeshPaths(meshPaths);
                return scenegraph;
            }
            else
            {
                throw runtime_error("Parsed scene graph, but nothing set as root");
            }
        }

    protected:
        virtual void parseTextureImage(istream &input)
        {
            string varname, filename;
            input >> varname >> filename;
            cout << "Read " << varname << " " << filename << endl;
            ImageLoader *loader = new PPMImageLoader();
            loader->load(filename);
            util::TextureImage *textureObject = new util::TextureImage(loader->getPixels(), loader->getWidth(), loader->getHeight(), varname);
            textureObjects[varname] = textureObject;
        }
        virtual void parseGroup(istream &input)
        {
            string varname, name;
            input >> varname >> name;
            cout << "Read " << varname << " " << name << endl;
            SGNode *group = new GroupNode(name, NULL);
            nodes[varname] = group;
        }

        virtual void parseLeaf(istream &input)
        {
            string varname, name, command, instanceof ;
            input >> varname >> name;
            cout << "Read " << varname << " " << name << endl;
            input >> command;

            if (command == "instanceof")
            {
                input >> instanceof ;
            }

            SGNode *leaf = new LeafNode(instanceof, name, NULL);
            nodes[varname] = leaf;
        }

        virtual void parseScale(istream &input)
        {
            string varname, name;
            input >> varname >> name;
            float sx, sy, sz;
            input >> sx >> sy >> sz;
            SGNode *scaleNode = new ScaleTransform(sx, sy, sz, name, NULL);
            nodes[varname] = scaleNode;
        }

        virtual void parseTranslate(istream &input)
        {
            string varname, name;
            input >> varname >> name;
            float tx, ty, tz;
            input >> tx >> ty >> tz;
            SGNode *translateNode = new TranslateTransform(tx, ty, tz, name, NULL);
            nodes[varname] = translateNode;
        }

        virtual void parseLight(istream &input)
        {
            /*
                color
                position = glm::vec4(0, 0, 0, 1);
                spotDirection = glm::vec4(0, 0, 0, 0);
                spotCutoff = 0.0f;

                example light xml:
                    light light1
                    position 0 0 0
                    direction 0 0 1
                    ambient 0.611 0.349 0.207
                    diffuse 0.4 0.4 0.6
                    specular 0 0 0.8
                    spot-direction 0 0 0
                    spot-angle 23.0
                    end-light
            */
            util::Light light;
            float r, g, b;
            float x, y, z;
            float angle;
            string name;
            input >> name;
            // printf("\nlight: %s\n", name.c_str());
            string command;
            input >> command;
            while (command != "end-light")
            {
                std::cout << "\nParser.parseLight WHILE LOOP: \nname: "
                          << name
                          << "\nPOSITION: " << light.getPosition()
                          << "\nAMBIENT: " << light.getAmbient()
                          << "\nDIFFUSE: " << light.getDiffuse()
                          << "\nSPECULAR: " << light.getSpecular()
                          << "\nSPOT-DIRECTION: " << light.getSpotDirection()
                          << "\nSPOT-ANGLE: " << light.getSpotCutoff()
                          << "\n"
                          << std::endl;
                if (command == "position")
                {
                    input >> x >> y >> z;
                    // printf("pos: %f, %f, %f\n", x, y, z);
                    light.setPosition(x, y, z);
                    // light.setPosition(glm::vec4(x, y, z, 1.0f));
                    // std::cout << "AFTER INITIAL SETTING Parser.parseLight: lights[name]: " << name << ", pos: " << light.getPosition() << std::endl;
                }
                else if (command == "direction")
                {
                    input >> x >> y >> z;
                    // printf("dir: %f, %f, %f\n", x, y, z);
                    light.setDirection(x, y, z);
                }
                else if (command == "ambient")
                {
                    input >> r >> g >> b;
                    // printf("ambient: %f, %f, %f\n", r, g, b);
                    light.setAmbient(r, g, b);
                    // std::cout << "AFTER INITIAL SETTING Parser.parseLight: lights[name]: " << name << ", ambient: " << light.getAmbient() << std::endl;
                }
                else if (command == "diffuse")
                {
                    input >> r >> g >> b;
                    // printf("diffuse: %f, %f, %f\n", r, g, b);
                    light.setDiffuse(r, g, b);
                }
                else if (command == "specular")
                {
                    input >> r >> g >> b;
                    // printf("specular: %f, %f, %f\n", r, g, b);
                    light.setSpecular(r, g, b);
                }
                else if (command == "spot-direction")
                {
                    input >> x >> y >> z;
                    // printf("spot-direction: %f, %f, %f\n", x, y, z);
                    light.setSpotDirection(x, y, z);
                }
                else if (command == "spot-angle")
                {
                    input >> angle;
                    // printf("spot-angle: %f\n", angle);
                    light.setSpotAngle(angle);
                    light.setIsSpotlight(true);
                }
                input >> command;
            }

            // std::cout << "BEFORE SETTING Parser.parseLight: light: " << name << ", pos: " << light.getPosition() << std::endl;
            // std::cout << "BEFORE SETTING Parser.parseLight: light: " << name << ", ambient: " << light.getAmbient() << std::endl;
            // std::cout << "BEFORE SETTING Parser.parseLight: light: " << name << ", diffuse: " << light.getDiffuse() << std::endl;
            // std::cout << "BEFORE SETTING Parser.parseLight: light: " << name << ", specular: " << light.getSpecular() << std::endl;
            lights[name] = light;
            // std::cout << lights[name] << endl;
            // std::cout << "Parser.parseLight: lights[name]: " << name << ", pos: " << lights[name].getPosition() << std::endl;
            // std::cout << "Parser.parseLight: lights[name]: " << name << ", ambient: " << lights[name].getAmbient() << std::endl;
            // std::cout << "Parser.parseLight: lights[name]: " << name << ", diffuse: " << lights[name].getDiffuse() << std::endl;
            // std::cout << "Parser.parseLight: lights[name]: " << name << ", specular: " << lights[name].getSpecular() << std::endl;
        }

        virtual void parseAttachLight(istream &input)
        {
            string nodename, lightname;
            input >> nodename >> lightname;

            SGNode *node = NULL;
            util::Light light;

            // printf("\nIN parseAttachLight, checking if nodename, lightname %s exists...\n", lightname.c_str());
            // printf("first element in lights %s\n", lights.begin()->first.c_str());
            // printf("first element in nodes %s\n", nodes.begin()->first.c_str());
            if ((nodes.find(nodename) != nodes.end()) && (lights.find(lightname) != lights.end()))
            {
                light = lights[lightname];

                // printf("found %s, attaching light...\n", lightname.c_str());
                // std::cout << light << endl;
                std::cout << "parser.parseAttachLight(): pos: " << light.getPosition() << std::endl;

                nodes[nodename]->attachLight(lightname, light);
                // printf("out from attachLight\n");
            }
        }

        virtual void parseRotate(istream &input)
        {
            string varname, name;
            input >> varname >> name;
            float angleInDegrees, ax, ay, az;
            input >> angleInDegrees >> ax >> ay >> az;
            SGNode *rotateNode = new RotateTransform(glm::radians(angleInDegrees), ax, ay, az, name, NULL);
            nodes[varname] = rotateNode;
        }

        virtual void parseMaterial(istream &input)
        {
            util::Material mat;
            float r, g, b;
            string name;
            input >> name;
            string command;
            input >> command;
            while (command != "end-material")
            {
                if (command == "ambient")
                {
                    input >> r >> g >> b;
                    mat.setAmbient(r, g, b);
                }
                else if (command == "diffuse")
                {
                    input >> r >> g >> b;
                    mat.setDiffuse(r, g, b);
                }
                else if (command == "specular")
                {
                    input >> r >> g >> b;
                    mat.setSpecular(r, g, b);
                }
                else if (command == "emission")
                {
                    input >> r >> g >> b;
                    mat.setEmission(r, g, b);
                }
                else if (command == "shininess")
                {
                    input >> r;
                    mat.setShininess(r);
                }
                input >> command;
            }
            materials[name] = mat;
        }

        virtual void parseCopy(istream &input)
        {
            string nodename, copyof;

            input >> nodename >> copyof;
            if (nodes.find(copyof) != nodes.end())
            {
                SGNode *copy = nodes[copyof]->clone();
                nodes[nodename] = copy;
            }
        }

        virtual void parseImport(istream &input)
        {
            string nodename, filepath;

            input >> nodename >> filepath;
            ifstream external_scenegraph_file(filepath);
            if (external_scenegraph_file.is_open())
            {

                IScenegraph *importedSG = parse(external_scenegraph_file);
                nodes[nodename] = importedSG->getRoot();
                /* for (map<string,util::PolygonMesh<VertexAttrib> >::iterator it=importedSG->getMeshes().begin();it!=importedSG->getMeshes().end();it++) {
                     this->meshes[it->first] = it->second;
                 }
                 for (map<string,string>::iterator it=importedSG->getMeshPaths().begin();it!=importedSG->getMeshPaths().end();it++) {
                     this->meshPaths[it->first] = it->second;
                 }
                 */
                // delete the imported scene graph but not its nodes!
                importedSG->makeScenegraph(NULL);
                delete importedSG;
            }
        }

        virtual void parseAssignTexture(istream &input)
        {
            /**
             * texture-image earthmap textures/earthmap.ppm
             * assign-texture node-3-0 earthmap
             */
            string nodename, texturename;
            input >> nodename >> texturename;

            LeafNode *leafNode = dynamic_cast<LeafNode *>(nodes[nodename]);
            if ((leafNode != NULL) && (textureObjects.find(texturename) != textureObjects.end()))
            {
                leafNode->setTexture(textureObjects[texturename]);
            }
        }
        virtual void parseAssignMaterial(istream &input)
        {
            string nodename, matname;
            input >> nodename >> matname;

            LeafNode *leafNode = dynamic_cast<LeafNode *>(nodes[nodename]);
            if ((leafNode != NULL) && (materials.find(matname) != materials.end()))
            {
                leafNode->setMaterial(materials[matname]);
            }
        }

        virtual void parseAddChild(istream &input)
        {
            string childname, parentname;

            input >> childname >> parentname;
            ParentSGNode *parentNode = dynamic_cast<ParentSGNode *>(nodes[parentname]);
            SGNode *childNode = NULL;
            if (nodes.find(childname) != nodes.end())
            {
                childNode = nodes[childname];
            }

            if ((parentNode != NULL) && (childNode != NULL))
            {
                parentNode->addChild(childNode);
            }
        }

        virtual void parseSetRoot(istream &input)
        {
            string rootname;
            input >> rootname;

            root = nodes[rootname];

            cout << "Root's name is " << root->getName() << endl;
        }

        string stripComments(istream &input)
        {
            string line;
            stringstream clean;
            while (getline(input, line))
            {
                int i = 0;
                while ((i < line.length()) && (line[i] != '#'))
                {
                    clean << line[i];
                    i++;
                }
                clean << endl;
            }
            return clean.str();
        }

    private:
        map<string, SGNode *> nodes;
        map<string, util::Material> materials;
        map<string, util::PolygonMesh<VertexAttrib>> meshes;
        map<string, util::Light> lights;
        map<string, util::TextureImage *> textureObjects;
        map<string, string> meshPaths;
        SGNode *root;
    };
}

#endif