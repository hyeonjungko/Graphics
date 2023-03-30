#ifndef _SCENEGRAPHLIGHTPOSCALCULATOR_H_
#define _SCENEGRAPHLIGHTPOSCALCULATOR_H_

#include "../ImageLoader.h"
#include "../PPMImageLoader.h"
#include "TextureImage.h"
#include "SGNodeVisitor.h"
#include "GroupNode.h"
#include "LeafNode.h"
#include "TransformNode.h"
#include "RotateTransform.h"
#include "ScaleTransform.h"
#include "TranslateTransform.h"
#include <ShaderProgram.h>
#include <ShaderLocationsVault.h>
#include "ObjectInstance.h"
#include "Light.h"
#include <stack>
#include <iostream>

#include <glm/gtx/string_cast.hpp> //

using namespace std;

namespace sgraph
{
    /**
     * This visitor implements drawing the scene graph using OpenGL
     *
     */
    class ScenegraphLightPosCalculator : public SGNodeVisitor
    {
    public:
        ScenegraphLightPosCalculator(stack<glm::mat4> &mv) : modelview(mv)
        {
        }
        ~ScenegraphLightPosCalculator()
        {
            // while (!modelview.empty())
            // {
            //     modelview.pop();
            // }
        }

        vector<util::Light> getScenegraphLights()
        {
            vector<util::Light> justLights;
            for (auto light : lights)
            {
                justLights.push_back(light.second);
            }

            return justLights;
        }

        void visitGroupNode(GroupNode *groupNode)
        {
            for (int i = 0; i < groupNode->getChildren().size(); i = i + 1)
            {
                groupNode->getChildren()[i]->accept(this);
            }

            printf("looking for lights in group node\n");
            // for each light in group node, update position
            for (auto &lightFromNode : groupNode->getLights())
            {
                printf("found a light in group node\n");
                printf("able to find the name of this light %s\n", lightFromNode.first.c_str());
                cout << "able to get position of this light " << lightFromNode.second.getPosition() << endl;
                glm::vec4 newPos = modelview.top() * glm::vec4((lightFromNode.second.getPosition()));
                glm::vec4 newSpotDir = modelview.top() * (lightFromNode.second.getSpotDirection());
                cout << "able to calculate new position of this light " << newPos << endl;
                cout << "able to calculate new spot direction of this light " << newSpotDir << endl;

                util::Light light = util::Light(lightFromNode.second);
                light.setPosition(newPos); // TODO: DONT set position of light in NODE, create a new Light object and add it to this Calc's map of Lights
                light.setSpotDirection(newSpotDir.x, newSpotDir.y, newSpotDir.z);
                cout << "adding light with new position " << light.getPosition() << endl;
                cout << "adding light with new spot direction " << light.getSpotDirection() << endl;

                lights[lightFromNode.first] = light;
                printf("updated a light's position in group node and added the new Light object to posCalculator's list of Lights\n");
            }
        }

        void visitLeafNode(LeafNode *leafNode)
        {
            // for each light in leaf node, update position
            for (auto &lightFromNode : leafNode->getLights())
            {
                glm::vec4 newPos = modelview.top() * lightFromNode.second.getPosition(); // TODO: convert to glm::vec4
                glm::vec4 newSpotDir = modelview.top() * (lightFromNode.second.getSpotDirection());

                util::Light light = util::Light(lightFromNode.second);
                light.setPosition(newPos);
                light.setSpotDirection(newSpotDir.x, newSpotDir.y, newSpotDir.z);
                lights[lightFromNode.first] = light;
            }
        }

        /**
         * @brief Multiply the transform to the modelview and recur to child
         *
         * @param transformNode
         */
        void visitTransformNode(TransformNode *transformNode)
        {
            modelview.push(modelview.top());
            modelview.top() = modelview.top() * transformNode->getTransform();
            if (transformNode->getChildren().size() > 0)
            {
                transformNode->getChildren()[0]->accept(this);
            }
            // printf("in visitTransformNode before pop\n");
            modelview.pop();
            // printf("in visitTransformNode after pop\n");
        }

        void visitScaleTransform(ScaleTransform *scaleNode)
        {
            visitTransformNode(scaleNode);
        }

        void visitTranslateTransform(TranslateTransform *translateNode)
        {
            visitTransformNode(translateNode);
        }

        void visitRotateTransform(RotateTransform *rotateNode)
        {
            visitTransformNode(rotateNode);
        }

    private:
        stack<glm::mat4> &modelview;
        util::ShaderLocationsVault shaderLocations;
        map<string, util::ObjectInstance *> objects;
        map<string, util::Light> lights;
    };
}

#endif