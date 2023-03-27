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
#include <stack>
#include <iostream>
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

        vector<util::Light> getScenegraphLights()
        {
            vector<util::Light> justLights;
            for (auto light : lights)
            {
                justLights.push_back(*light.second);
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
            for (auto &light : groupNode->getLights())
            {
                printf("found a light in group node\n");
                glm::vec4 newPos = modelview.top() * (light.second->getPosition());
                printf("calculated new position\n");
                light.second->setPosition(newPos);
                lights[light.first] = light.second;
                printf("updated a light's position in group node\n");
            }
        }

        void visitLeafNode(LeafNode *leafNode)
        {
            // for each light in leaf node, update position
            for (auto &light : leafNode->getLights())
            {
                glm::vec4 newPos = modelview.top() * light.second->getPosition();
                lights[light.first]->setPosition(newPos);
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
            // TODO: can lights be attached to Transform node?
            printf("in visitTransformNode before pop\n");
            modelview.pop();
            printf("in visitTransformNode after pop\n");
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
        map<string, util::Light *> lights;
    };
}

#endif