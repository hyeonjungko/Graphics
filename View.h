#ifndef __VIEW_H__
#define __VIEW_H__

#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cstdio>
#include <ShaderProgram.h>
#include "sgraph/SGNodeVisitor.h"
#include "sgraph/ScenegraphLightPosCalculator.h"
#include "ObjectInstance.h"
// #include "PolygonMesh.h"
#include "VertexAttrib.h"
#include "Callbacks.h"
#include "sgraph/IScenegraph.h"
#include "Model.h"
#include "raytracer/Ray.h"
#include "raytracer/HitRecord.h"

#include <stack>
using namespace std;

class View
{
    typedef enum
    {
        GLOBAL,
        CHOPPER,
        FPS
    } TypeOfCamera;

    class LightLocation
    {
    public:
        int ambient, diffuse, specular, position, spotAngle, spotDirection, isSpotlight;
        LightLocation()
        {
            ambient = diffuse = specular = position = spotAngle = spotDirection = isSpotlight = -1;
        }
    };

public:
    View();
    ~View();
    // void init(Callbacks *callbacks, map<string, util::PolygonMesh<VertexAttrib>> &meshes);
    void init(Callbacks *callbacks, Model &model);
    void display(sgraph::IScenegraph *scenegraph);
    glm::vec4 getPixelColor(raytracer::Ray ray, int maxBounces);
    glm::vec4 calcReflect(raytracer::HitRecord hit, raytracer::Ray ray, int maxBounces);
    glm::vec4 shadeAll(raytracer::HitRecord hit);
    glm::vec4 calcLight(util::Light light, raytracer::HitRecord hit);
    glm::vec4 calcLightAmbientOnly(util::Light light, raytracer::HitRecord hit);
    glm::vec4 shade(raytracer::HitRecord hit, util::Light light);
    void raytrace(sgraph::IScenegraph *scenegraph, int w, int h, stack<glm::mat4> modelview);
    void setProjection(int width, int height);
    tuple<float, float> getCurrentMousePositions();
    void updateTrackBallRotation(float xDelta, float yDelta);
    bool shouldWindowClose();
    void closeWindow();
    void resetTrackBall();
    void setGlobal();
    void setChopper();
    void setFPS();
    void moveForward();
    void moveBackward();
    void turnLeft();
    void turnRight();
    void turnUp();
    void turnDown();
    void setFrustumVertices();
    void calculateLightPos(sgraph::IScenegraph *scenegraph); //
    void initLightShaderVars();
    void toggleRaytraceMode();

private:
    GLFWwindow *window;
    util::ShaderProgram program;
    util::ShaderLocationsVault shaderLocations;
    map<string, util::ObjectInstance *> objects;
    sgraph::IScenegraph *scenegraph;
    vector<util::Light> lights;
    vector<LightLocation> lightLocations;
    glm::mat4 projection;
    stack<glm::mat4> modelview;
    sgraph::SGNodeVisitor *renderer;
    sgraph::ScenegraphLightPosCalculator *lightPosCalculator;
    int frames;
    double time;
    float xRotAngle;
    float yRotAngle;
    TypeOfCamera cameraMode;
    glm::vec3 personEyePosition, personDirection, personUp;
    float left;
    float up;
    float aspectRatio;
    bool raytraceEnabled;
    int MAX_BOUNCE;
    int currBounceCount;

    vector<util::PolygonMesh<VertexAttrib>> frustum_meshes;
    vector<util::ObjectInstance *> frustum_objects;
};

#endif