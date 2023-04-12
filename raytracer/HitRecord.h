#ifndef __HITRECORD_H__
#define __HITRECORD_H__

#include "glm/glm.hpp"
#include "Material.h"
#include "TextureImage.h"
#include <cmath>

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
        HitRecord()
        {
            t = INFINITY;
        }
        inline float getT() const;
        inline glm::vec4 getIntersection() const;
        inline glm::vec3 getNormal() const;
        inline util::Material getMaterial() const;
        inline util::TextureImage getTexture() const;

        inline void setT(float time);
        inline void setIntersection(float x, float y, float z);
        inline void setNormal(float x, float y, float z);
        inline void setNormal(const glm::vec3 &norm);
        inline void setMaterial(const util::Material &mat);
        inline void setTextureImage(const util::TextureImage &texture);

    private:
        float t;
        glm::vec4 intersection;
        glm::vec3 normal;
        util::Material material;
        util::TextureImage texture;
        // TODO: need to add Texture coordinates
    };

    float HitRecord::getT() const
    {
        return t;
    }
    glm::vec4 HitRecord::getIntersection() const
    {
        return glm::vec4(intersection);
    }
    glm::vec3 HitRecord::getNormal() const
    {
        return glm::vec3(normal);
    }
    util::Material HitRecord::getMaterial() const
    {
        return material;
    }
    util::TextureImage HitRecord::getTexture() const
    {
        return texture;
    }
    void HitRecord::setT(float time)
    {
        t = time;
    }
    void HitRecord::setIntersection(float x, float y, float z)
    {
        intersection = glm::vec4(x, y, z, 1);
    }
    void HitRecord::setNormal(float x, float y, float z)
    {
        normal = glm::vec3(x, y, z);
    }
    void HitRecord::setNormal(const glm::vec3 &norm)
    {
        normal = glm::vec3(norm);
    }
    void HitRecord::setMaterial(const util::Material &mat)
    {
        material = mat;
    }
    void HitRecord::setTextureImage(const util::TextureImage &textureImage)
    {
        texture = textureImage;
    }
}
#endif
