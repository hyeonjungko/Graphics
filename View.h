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
#include "ObjectInstance.h"
#include "PolygonMesh.h"
#include "VertexAttrib.h"
#include "Callbacks.h"
#include "sgraph/IScenegraph.h"

#include <stack>
using namespace std;

class View
{
    typedef enum {GLOBAL, CHOPPER, FPS} TypeOfCamera;
public:
    View();
    ~View();
    void init(Callbacks *callbacks, map<string, util::PolygonMesh<VertexAttrib>> &meshes);
    void display(sgraph::IScenegraph *scenegraph);
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
private:
    GLFWwindow *window;
    util::ShaderProgram program;
    util::ShaderLocationsVault shaderLocations;
    map<string, util::ObjectInstance *> objects;
    glm::mat4 projection;
    stack<glm::mat4> modelview;
    sgraph::SGNodeVisitor *renderer;
    int frames;
    double time;
    float xRotAngle;
    float yRotAngle; 
    TypeOfCamera cameraMode;
    glm::vec3 personEyePosition, personDirection, personUp;
    float left;
    float up; 
    float aspectRatio;
    
    vector<util::PolygonMesh<VertexAttrib>> frustum_meshes;
    vector<util::ObjectInstance *> frustum_objects;
};

#endif