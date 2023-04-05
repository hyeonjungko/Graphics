#ifndef __3DRAY_H__
#define __3DRAY_H__

#include <glm/glm.hpp>

namespace raytracer
{
    class 3DRay
    {
        /*
         * contains a starting 3D point and a direction as a 3D vector.
         */
    public:
        inline glm::vec3 getStartingPoint() const;
        inline glm::vec3 getDirection() const;
        inline void setStartingPoint(float x, float y, float z) const;
        inline void setDirection(float x, float y, float z) const;

    private:
        glm::vec3 startingPoint;
        glm::vec3 direction;
    };

    void 3DRay ::setStartingPoint(float x, float y, float z)
    {
        startingPoint = glm::vec3(x, y, z);
    }
    void 3DRay ::setDirection(float x, float y, float z)
    {
        direction = glm::vec3(x, y, z);
    }
}
#endif
