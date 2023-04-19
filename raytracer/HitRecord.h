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
            textureName = "white";
            texColor = glm::vec4(1, 1, 1, 1);
        }
        inline float getT() const;
        inline glm::vec4 getIntersection() const;
        inline glm::vec3 getNormal() const;
        inline util::Material getMaterial() const;
        inline string getTextureName() const;
        inline glm::vec2 getTextCoord() const;
        inline glm::vec4 getTextColor() const;

        inline void setT(float time);
        inline void setTextCoord(const glm::vec2 &texCoord);
        inline void setIntersection(float x, float y, float z);
        inline void setNormal(float x, float y, float z);
        inline void setNormal(const glm::vec3 &norm);
        inline void setMaterial(const util::Material &mat);
        inline void setTextureName(const string textureName);
        inline void setTextureColor(const glm::vec4 &textureColor);

    private:
        float t;
        glm::vec4 intersection;
        glm::vec3 normal;
        util::Material material;
        string textureName;
        glm::vec2 texcoord;
        glm::vec4 texColor;
    };

    glm::vec4 HitRecord::getTextColor() const
    {
        return glm::vec4(texColor);
    }
    string HitRecord::getTextureName() const
    {
        return textureName;
    }
    glm::vec2 HitRecord::getTextCoord() const
    {
        return glm::vec2(texcoord);
    }
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
    void HitRecord::setT(float time)
    {
        t = time;
    }

    void HitRecord::setTextureColor(const glm::vec4 &textureColor)
    {
        texColor = glm::vec4(textureColor);
    }

    void HitRecord::setTextCoord(const glm::vec2 &texCoord)
    {
        texcoord = glm::vec2(texCoord);
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
    void HitRecord::setTextureName(const string textureImageName)
    {
        textureName = textureImageName;
    }
}
#endif
