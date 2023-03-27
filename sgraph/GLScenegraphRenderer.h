#ifndef _GLSCENEGRAPHRENDERER_H_
#define _GLSCENEGRAPHRENDERER_H_

#include "../ImageLoader.h"
#include "../PPMImageLoader.h"
#include "../TextureImage.h"
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
    class GLScenegraphRenderer : public SGNodeVisitor
    {
    public:
        /**
         * @brief Construct a new GLScenegraphRenderer object
         *
         * @param mv a reference to modelview stack that will be used while rendering
         * @param os the map of ObjectInstance objects
         * @param shaderLocations the shader locations for the program used to render
         */
        GLScenegraphRenderer(stack<glm::mat4> &mv, map<string, util::ObjectInstance *> &os, util::ShaderLocationsVault &shaderLocations)
            : modelview(mv), objects(os)
        {
            this->shaderLocations = shaderLocations;
            for (map<string, util::ObjectInstance *>::iterator it = objects.begin(); it != objects.end(); it++)
            {
                cout << "Mesh with name: " << it->first << endl;
            }
        }

        /**
         * @brief Recur to the children for drawing
         *
         * @param groupNode
         */
        void visitGroupNode(GroupNode *groupNode)
        {
            for (int i = 0; i < groupNode->getChildren().size(); i = i + 1)
            {
                groupNode->getChildren()[i]->accept(this);
            }
        }

        /**
         * @brief Draw the instance for the leaf, after passing the
         * modelview and color to the shader
         *
         * @param leafNode
         */
        void visitLeafNode(LeafNode *leafNode)
        {

            // load in the textures TODO: this is an intermediate step
            ImageLoader *loader = new PPMImageLoader();
            loader->load("textures/earthmap.ppm");
            util::TextureImage *textureObject = new util::TextureImage(loader->getPixels(), loader->getWidth(), loader->getHeight(), "earthmap");

            unsigned int textureId;
            glGenTextures(1, &textureId);
            glBindTexture(GL_TEXTURE_2D, textureId);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // if the s-coordinate goes outside (0,1), repeat it
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // if the t-coordinate goes outside (0,1), repeat it
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureObject->getWidth(), textureObject->getHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, textureObject->getImage());
            glGenerateMipmap(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, textureId);
            /**
             * in vec4 vPosition;
             * in vec4 vNormal;
             * in vec4 vTexCoord;
             *
             * uniform mat4 projection;
             * uniform mat4 modelview;
             * uniform mat4 normalmatrix;
             * uniform mat4 texturematrix;
             */
            glUniformMatrix4fv(shaderLocations.getLocation("modelview"), 1, GL_FALSE, glm::value_ptr(modelview.top()));
            glm::mat4 normalmatrix = glm::inverse(glm::transpose((modelview.top())));
            glUniformMatrix4fv(shaderLocations.getLocation("normalmatrix"), 1, false, glm::value_ptr(normalmatrix));
            glm::mat4 textureTransform = glm::mat4(1.0);
            glUniformMatrix4fv(shaderLocations.getLocation("texturematrix"), 1, false, glm::value_ptr(textureTransform));

            // enable texture mapping
            glEnable(GL_TEXTURE_2D);
            glActiveTexture(GL_TEXTURE0);
            // tell the shader to look for GL_TEXTURE"0"
            glUniform1i(shaderLocations.getLocation("image"), 0); // TODO: need functions to fetch texture images

            glUniform3fv(shaderLocations.getLocation("material.ambient"), 1, glm::value_ptr(leafNode->getMaterial().getAmbient()));
            glUniform3fv(shaderLocations.getLocation("material.diffuse"), 1, glm::value_ptr(leafNode->getMaterial().getDiffuse()));
            glUniform3fv(shaderLocations.getLocation("material.specular"), 1, glm::value_ptr(leafNode->getMaterial().getSpecular()));
            glUniform1f(shaderLocations.getLocation("material.shininess"), leafNode->getMaterial().getShininess());

            objects[leafNode->getInstanceOf()]->draw();
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
            modelview.pop();
        }

        /**
         * @brief For this visitor, only the transformation matrix is required.
         * Thus there is nothing special to be done for each type of transformation.
         * We delegate to visitTransformNode above
         *
         * @param scaleNode
         */
        void visitScaleTransform(ScaleTransform *scaleNode)
        {
            visitTransformNode(scaleNode);
        }

        /**
         * @brief For this visitor, only the transformation matrix is required.
         * Thus there is nothing special to be done for each type of transformation.
         * We delegate to visitTransformNode above
         *
         * @param translateNode
         */
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
    };
}

#endif