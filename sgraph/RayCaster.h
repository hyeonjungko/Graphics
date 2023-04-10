#ifndef _RayCaster_H_
#define _RayCaster_H_

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
#include "../raytracer/Ray.h"
#include "../raytracer/HitRecord.h"
#include <stack>
#include <iostream>

#include <glm/gtx/string_cast.hpp> //

using namespace std;

namespace sgraph
{
    /**
     * This visitor implements ray caster
     *
     */
    class RayCaster : public SGNodeVisitor
    {
    public:
        RayCaster(stack<glm::mat4> mv, raytracer::Ray r, map<string, util::ObjectInstance *> &os) : objects(os), ray(r), modelview(mv)
        {
            hit = raytracer::HitRecord();
            cout << "in RayCaster(), modelview.top(): " << modelview.top() << endl;
        }
        ~RayCaster()
        {
        }

        raytracer::HitRecord getHitRecord()
        {
            return hit;
        }

        void visitGroupNode(GroupNode *groupNode)
        {
            for (int i = 0; i < groupNode->getChildren().size(); i = i + 1)
            {
                groupNode->getChildren()[i]->accept(this);
            }
        }

        /**
         * determine if the ray in this raycaster intersects with the sphere object
         * if so, set t
         */
        bool sphereIntersect(glm::vec4 &orig, glm::vec4 &rayDir, float &t)
        {
            float t0, t1;
            float a = glm::dot(rayDir, rayDir); // should be 1 if ray directions were normalized
            float b = 2 * glm::dot(rayDir, orig);
            float c = orig.x * orig.x + orig.y * orig.y + orig.z * orig.z - 1; // 1 because radius^2 is 1

            cout << "in sphereIntersect, rayDir: " << rayDir << " rayOrig: " << orig << endl;
            cout << "in sphereIntersect, a: " << a << " b: " << b << " c: " << c << endl;

            float discr = b * b - 4 * a * c;

            if (discr < 0)
            {
                // no intersection
                return false;
            }
            else if (discr == 0)
            {
                // only one solution for t. ray grazes the object's surface
                t0 = t1 = -0.5 * b / a;
            }
            else
            {
                // two solutions for t. ray goes through the object
                // float q = (b > 0) ? -0.5 * (b + sqrt(discr)) : -0.5 * (b - sqrt(discr));
                // t0 = q / a;
                // t1 = c / q;
                t0 = (-b + sqrt(discr)) / (2 * a);
                t1 = (-b - sqrt(discr)) / (2 * a);
            }

            // determine the closest positive t value if it exists
            if (t0 > t1)
            {
                std::swap(t0, t1);
            }

            if (t0 < 0)
            {
                t0 = t1; // if t0 is negative, let's use t1 instead
                if (t0 < 0)
                {
                    // both t0 and t1 are negative, ray does not intersect the object in front
                    // no intersection
                    return false;
                }
            }
            // found t
            t = t0;
            cout << "in sphereIntersect, found t: " << t << endl;
            return true;
        }
        /**
         * determine if the ray in this raycaster intersects with the box object
         * if so, set t
         */
        bool boxIntersect(glm::vec4 &orig, glm::vec4 &rayInvDir, float &t)
        {
            float tmin, tmax, tymin, tymax, tzmin, tzmax;

            tmin = (-0.5f - orig.x) * rayInvDir.x;
            tmax = (0.5f - orig.x) * rayInvDir.x;
            tymin = (-0.5f - orig.y) * rayInvDir.y;
            tymax = (0.5f - orig.y) * rayInvDir.y;

            if ((tmin > tymax) || (tymin > tmax))
                return false;

            if (tymin > tmin)
                tmin = tymin;
            if (tymax < tmax)
                tmax = tymax;

            tzmin = (-0.5f - orig.z) * rayInvDir.z;
            tzmax = (0.5f - orig.z) * rayInvDir.z;

            if ((tmin > tzmax) || (tzmin > tmax))
                return false;

            if (tzmin > tmin)
                tmin = tzmin;
            if (tzmax < tmax)
                tmax = tzmax;

            t = tmin;

            if (t < 0)
            {
                t = tmax;
                if (t < 0)
                    return false;
            }

            return true;
        }

        void visitLeafNode(LeafNode *leafNode)
        {
            // for object in LeafNode,
            // calculate if there is an intersection between the object & this caster's ray

            // multiply the inverse of the modelview.top() to get the ray's origin and direction from
            // view coordinate system into object's coordinate system
            glm::vec3 color;
            float t;
            glm::vec4 hitPos;
            glm::vec4 hitNorm;
            glm::vec4 orig = glm::inverse(modelview.top()) * ray.getOrigin(); // ray origin position now in object coordinate system
            glm::vec4 rayDir = glm::inverse(modelview.top()) * ray.getDir();  // ray direction now in object coordinate system
            glm::vec4 rayInvDir = 1.0f / rayDir;

            cout << "in visitLeafNode, rayDir: " << rayDir << " rayOrig: " << orig << endl;
            cout << "in visitLeafNode, modelview.top(): " << modelview.top() << endl;

            bool objIsSphere = leafNode->getInstanceOf() == "sphere";
            bool objIsBox = leafNode->getInstanceOf() == "box";

            if ((objIsSphere && sphereIntersect(orig, rayDir, t)) ||
                (objIsBox && boxIntersect(orig, rayInvDir, t)))
            {
                hitPos = orig + rayDir * t;
                // update intersection position and normal from object coordinate system back to view coordinate system
                hitPos = hitPos * modelview.top();
                hitNorm = normalize(hitPos); // center at (0,0,0) so no subtraction

                // set HitRecord in view coordinate system
                hit.setT(t);
                hit.setIntersection(hitPos.x, hitPos.y, hitPos.z);
                hit.setNormal(hitNorm);
                // hit.setMaterial(const util::Material &mat);             // TODO:
                // hit.setTextureImage(const util::TextureImage &texture); // TODO:
                cout << "found intersection " << hit.getT() << endl;
            }
            else
            {
                cout << "in else " << hit.getT() << endl;
                // no intersection, no updates on hitRecord
            }
            // there may be multiple HitRecords as this raycaster with this particular ray
            // descends down the scenegraph.
            // TODO: Q: how to determine the correct HitRecord? -> probably one with the smallest positive t value?
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

        glm::vec3 getPixelColor()
        {
            float hitT = hit.getT();
            if (hitT == INFINITY)
            {
                // return default bgColor
                return glm::vec3(66, 135, 245);
            }
            else
            {
                // TODO: calculate and return color for current HitRecord
                //
                return glm::vec3(245, 173, 66);
            }
        }

    private:
        stack<glm::mat4> modelview;
        raytracer::Ray ray;
        raytracer::HitRecord hit;

        util::ShaderLocationsVault shaderLocations;
        map<string, util::ObjectInstance *> objects;
    };
}

#endif