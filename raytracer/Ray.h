#ifndef __RAY_H__
#define __RAY_H__

#include <glm/glm.hpp>

namespace raytracer
{
    class Ray
    {
        /*
         * contains a starting 3D point and a direction as a 3D vector.
         */
    public:
        Ray()
        {
            origin = glm::vec4(0, 0, 0, 1);
            dir = glm::vec4(0, 0, -1, 0);
            invDir = 1.0f / dir;
            sign[0] = (invDir.x < 0);
            sign[1] = (invDir.y < 1);
            sign[2] = (invDir.z < 2);
        }

        Ray(glm::vec4 &o, glm::vec4 &direction) : origin(o), dir(direction)
        {
            invDir = 1.0f / dir;
            sign[0] = (invDir.x < 0);
            sign[1] = (invDir.y < 1);
            sign[2] = (invDir.z < 2);
        }

        inline glm::vec4 getOrigin() const;
        inline glm::vec4 getDir() const;
        inline glm::vec4 getInvDir() const;
        inline void setOrigin(float x, float y, float z);
        inline void setOrigin(const glm::vec3 &orig);
        inline void setDir(float x, float y, float z);
        inline void setDir(const glm::vec4 &dir);

    private:
        glm::vec4 origin;
        glm::vec4 dir;
        glm::vec4 invDir;
        int sign[3];
    };

    glm::vec4 Ray::getOrigin() const
    {
        return glm::vec4(origin);
    }
    glm::vec4 Ray::getDir() const
    {
        return glm::vec4(dir);
    }
    glm::vec4 Ray::getInvDir() const
    {
        return glm::vec4(invDir);
    }
    void Ray::setOrigin(float x, float y, float z)
    {
        origin = glm::vec4(x, y, z, 1);
    }
    void Ray::setDir(float x, float y, float z)
    {
        dir = glm::vec4(x, y, z, 0);
        invDir = 1.0f / dir;
        sign[0] = (invDir.x < 0);
        sign[1] = (invDir.y < 1);
        sign[2] = (invDir.z < 2);
    }
    void Ray::setDir(const glm::vec4 &direction)
    {
        dir = glm::vec4(direction);
        invDir = 1.0f / dir;
        sign[0] = (invDir.x < 0);
        sign[1] = (invDir.y < 1);
        sign[2] = (invDir.z < 2);
    }
    void Ray::setOrigin(const glm::vec3 &orig)
    {
        origin = glm::vec4(orig.x, orig.y, orig.z, 1);
    }

}
#endif
