#ifndef _LEAFNODE_H_
#define _LEAFNODE_H_

#include "AbstractSGNode.h"
#include "SGNodeVisitor.h"
#include "Material.h"
#include "glm/glm.hpp"
#include "TextureImage.h"
#include <map>
#include <stack>
#include <string>
using namespace std;

namespace sgraph
{

    /**
     * This node represents the leaf of a scene graph. It is the only type of node that has
     * actual geometry to render.
     * \author Amit Shesh
     */
    class LeafNode : public AbstractSGNode
    {
        /**
         * The name of the object instance that this leaf contains. All object instances are stored
         * in the scene graph itself, so that an instance can be reused in several leaves
         */

    protected:
        string objInstanceName;
        /**
         * The material associated with the object instance at this leaf
         */
        util::Material material;
        util::TextureImage *texture;

    public:
        LeafNode(const string &instanceOf, util::Material &material, const string &name, sgraph::IScenegraph *graph)
            : AbstractSGNode(name, graph)
        {
            this->objInstanceName = instanceOf;
            this->material = material;

            ImageLoader *loader = new PPMImageLoader();
            loader->load("textures/all-white.ppm");
            util::TextureImage *textureObject = new util::TextureImage(loader->getPixels(), loader->getWidth(), loader->getHeight(), "all-white");
            this->texture = textureObject;
        }

        LeafNode(const string &instanceOf, const string &name, sgraph::IScenegraph *graph)
            : AbstractSGNode(name, graph)
        {
            this->objInstanceName = instanceOf;

            ImageLoader *loader = new PPMImageLoader();
            loader->load("textures/all-white.ppm");
            util::TextureImage *textureObject = new util::TextureImage(loader->getPixels(), loader->getWidth(), loader->getHeight(), "all-white");
            this->texture = textureObject;

            // empty default material
            this->material = util::Material();
        }

        ~LeafNode() {}

        /*
         *Set the material of each vertex in this object
         */
        void setMaterial(const util::Material &mat)
        {
            material = mat;
        }

        /*
         * gets the material
         */
        util::Material getMaterial()
        {
            return material;
        }
        /*
         *Set the texture image of each vertex in this object
         */
        void setTexture(util::TextureImage *textureImage)
        {
            texture = textureImage;
        }
        /*
         * gets the texture
         */
        util::TextureImage *getTexture()
        {
            return texture;
        }

        /**
         * Get the name of the instance this leaf contains
         *
         * @return string
         */
        string getInstanceOf()
        {
            return this->objInstanceName;
        }

        /**
         * Get a copy of this node.
         *
         * @return SGNode*
         */

        SGNode *clone()
        {
            LeafNode *newclone = new LeafNode(this->objInstanceName, material, name, scenegraph);
            return newclone;
        }

        /**
         * Visit this node.
         *
         */
        void accept(SGNodeVisitor *visitor)
        {
            // printf("in leaf node accept\n");
            visitor->visitLeafNode(this);
        }
    };
}
#endif
