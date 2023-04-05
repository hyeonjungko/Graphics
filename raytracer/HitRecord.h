#ifndef __HITRECORD_H__
#define __HITRECORD_H__

#include "glm/glm.hpp"
#include "Material.h"
#include "TextureImage.h"

namespace raytracer
{
    class HitRecord
    {
        /*
         * Stores all the information that you will need to determine the closest object that was hit,
         * and information about that object to calculate shading.
         * More specifically, it will contain
         * (a) the time ‘t’ on the ray where it intersects an object
         * (b) the 3D point of intersection in view coordinates
         * (c) the 3D normal of the object at that point in view coordinates
         * (d) The material properties.
         * (e) Texture coordinates and the texture object, if applicable.
         */
    public:
        inline float getTime() const;
        inline glm::vec3 getIntersection() const;
        inline glm::vec3 getNormal() const;
        inline util::Material getMaterial() const;
        inline util::TextureImage getTexture() const;

        inline void setT(float x, float y, float z);
        inline void setIntersection(float x, float y, float z);
        inline void setNormal(float x, float y, float z);
        inline void setMaterial(const util::Material &mat);
        inline void setTextureImage(const util::TextureImage &texture);

    private:
        float t;
        glm::vec3 intersection;
        glm::vec3 normal;
        Material material;
        TextureImage texture; // TODO: Q: what are "texture coordinates?"
    };

    float getTime() const
    {
        return t;
    }
    glm::vec3 getIntersection() const
    {
        return glm::vec3(intersection);
    }
    glm::vec3 getNormal() const
    {
        return glm::vec3(normal);
    }
    util::Material getMaterial() const
    {
        return material;
    }
    util::TextureImage getTexture() const
    {
        return texture;
    }
    void setT(float time)
    {
        t = time;
    }
    void setIntersection(float x, float y, float z)
    {
        intersection = glm::vec3(x, y, z);
    }
    void setNormal(float x, float y, float z)
    {
        normal = glm::vec3(x, y, z);
    }
    void setMaterial(const util::Material &mat)
    {
        material = mat;
    }
    void setTextureImage(const util::TextureImage &textureImage)
    {
        texture = textureImage;
    }
}
#endif
