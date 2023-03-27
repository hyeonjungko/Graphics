#include "View.h"
#include <cstdio>
#include <cstdlib>
#include <vector>
using namespace std;
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "sgraph/GLScenegraphRenderer.h"
#include "VertexAttrib.h"

View::View()
{
}

View::~View()
{
}

void View::init(Callbacks *callbacks, map<string, util::PolygonMesh<VertexAttrib>> &meshes)
{
    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    window = glfwCreateWindow(800, 800, "Hello GLFW: Per-vertex coloring", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwSetWindowUserPointer(window, (void *)callbacks);

    // using C++ functions as callbacks to a C-style library
    glfwSetKeyCallback(window,
                       [](GLFWwindow *window, int key, int scancode, int action, int mods)
                       {
                           reinterpret_cast<Callbacks *>(glfwGetWindowUserPointer(window))->onkey(key, scancode, action, mods);
                       });

    glfwSetMouseButtonCallback(window,
                               [](GLFWwindow *window, int button, int action, int mods)
                               {
                                   reinterpret_cast<Callbacks *>(glfwGetWindowUserPointer(window))->onmouse(button, action, mods);
                               });

    glfwSetCursorPosCallback(window,
                             [](GLFWwindow *window, double xpos, double ypos)
                             {
                                 reinterpret_cast<Callbacks *>(glfwGetWindowUserPointer(window))->oncursorpos(xpos, ypos);
                             });

    glfwSetWindowSizeCallback(window,
                              [](GLFWwindow *window, int width, int height)
                              {
                                  reinterpret_cast<Callbacks *>(glfwGetWindowUserPointer(window))->reshape(width, height);
                              });

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(1);

    // create the shader program
    program.createProgram(string("shaders/default.vert"), string("shaders/default.frag"));
    // program.createProgram(string("shaders/phong-multiple.vert"), string("shaders/phong-multiple.frag"));
    //  assuming it got created, get all the shader variables that it uses
    //  so we can initialize them at some point
    //  enable the shader program
    program.enable();
    shaderLocations = program.getAllShaderVariables();

    /* In the mesh, we have some attributes for each vertex. In the shader
     * we have variables for each vertex attribute. We have to provide a mapping
     * between attribute name in the mesh and corresponding shader variable
     name.
     *
     * This will allow us to use PolygonMesh with any shader program, without
     * assuming that the attribute names in the mesh and the names of
     * shader variables will be the same.

       We create such a shader variable -> vertex attribute mapping now
     */
    map<string, string> shaderVarsToVertexAttribs;

    shaderVarsToVertexAttribs["vPosition"] = "position";
    shaderVarsToVertexAttribs["vNormal"] = "normal";
    shaderVarsToVertexAttribs["vTexCoord"] = "texcoord";

    for (typename map<string, util::PolygonMesh<VertexAttrib>>::iterator it = meshes.begin();
         it != meshes.end();
         it++)
    {
        util::ObjectInstance *obj = new util::ObjectInstance(it->first);
        obj->initPolygonMesh(shaderLocations,
                             shaderVarsToVertexAttribs,
                             it->second);
        objects[it->first] = obj;
    }

    renderer = new sgraph::GLScenegraphRenderer(modelview, objects, shaderLocations);

    int window_width, window_height;
    aspectRatio = (float)window_width / window_height;
    glfwGetFramebufferSize(window, &window_width, &window_height);

    // prepare the projection matrix for perspective projection
    projection = glm::perspective(glm::radians(90.0f), (float)window_width / window_height, 0.1f, 10000.0f);

    glViewport(0, 0, window_width, window_height);

    frames = 0;
    time = glfwGetTime();

    cameraMode = GLOBAL;
    float left = 0.0f;
    float up = 0.0f;

    // for FPS
    personEyePosition = glm::vec3(0.0f, 25.0f, -10.0f);
    ; // assume person's eyes are at head height
    personDirection = glm::vec3(0.0f, 10.0f, 100.0f);
    personUp = glm::vec3(0.0f, 1.0f, 0.0f); // assume person's up direction is positive y-axis
}

void View::display(sgraph::IScenegraph *scenegraph)
{
    program.enable();
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    while (!modelview.empty())
    {
        modelview.pop();
    }

    modelview.push(glm::mat4(1.0));

    if (cameraMode == GLOBAL)
    {
        modelview.top() = modelview.top() *
                          glm::lookAt(glm::vec3(100.0f, 100.0f, 150.0f),
                                      glm::vec3(0.0f, 0.0f, 0.0f),
                                      personUp);
    }
    else if (cameraMode == CHOPPER)
    {
        float distance = 100.0f;                                                                                           // distance from the scene
        float angle = glfwGetTime() * 20.0f;                                                                               // angle of rotation around the scene
        glm::vec3 cameraPos = glm::vec3(distance * sin(glm::radians(angle)), 220.0f, distance * cos(glm::radians(angle))); // the camera is rotating counter-clockwise
        modelview.top() = modelview.top() *
                          glm::lookAt(cameraPos,                       // camera position
                                      glm::vec3(50.0f, 100.0f, 10.0f), // target position
                                      personUp);                       // rotate around y-axis
    }
    else if (cameraMode == FPS)
    {
        // glm::vec3 eyePosition = personEyePosition +  glm::vec3(0.0f, 0.0f, 1.0f) * -10.0f;
        // personEyePosition =  // offset the eye position in the direction the person is facing
        modelview.top() = modelview.top() * glm::lookAt(personEyePosition, personDirection, personUp);
        // modelview.push(modelview.top());
        modelview.top() = modelview.top() *
                          glm::rotate(glm::mat4(1.0), glm::radians(left), glm::vec3(0.0f, 1.0f, 0.0f)) *
                          glm::rotate(glm::mat4(1.0), glm::radians(up), glm::vec3(1.0f, 0.0f, 0.0f));
    }

    modelview.push(modelview.top());
    modelview.top() = modelview.top() *
                      glm::rotate(glm::mat4(1.0), glm::radians(yRotAngle), glm::vec3(1.0f, 0.0f, 0.0f)) *
                      glm::rotate(glm::mat4(1.0), glm::radians(xRotAngle), glm::vec3(0.0f, 1.0f, 0.0f));

    glUniformMatrix4fv(shaderLocations.getLocation("projection"), 1, GL_FALSE, glm::value_ptr(projection));

    // draw scene graph here
    scenegraph->getRoot()
        ->accept(renderer);

    glFlush();
    program.disable();
    glfwSwapBuffers(window);
    glfwPollEvents();
    frames++;
    double currenttime = glfwGetTime();
    if ((currenttime - time) > 1.0)
    {
        printf("Framerate: %2.0f\r", frames / (currenttime - time));
        frames = 0;
        time = currenttime;
    }
}

void View::setProjection(int width, int height)
{
    glfwGetFramebufferSize(window, &width, &height);
    projection = glm::perspective(glm::radians(100.0f), (float)width / height, 0.1f, 10000.0f);
}

tuple<float, float> View::getCurrentMousePositions()
{
    double currXPos, currYPos;
    glfwGetCursorPos(window, &currXPos, &currYPos);
    tuple<float, float> currMousePos = make_tuple((float)currXPos, (float)currYPos);

    return currMousePos;
}

void View::updateTrackBallRotation(float xRotAngle, float yRotAngle)
{
    this->xRotAngle = (float)xRotAngle;
    this->yRotAngle = (float)yRotAngle;
}

bool View::shouldWindowClose()
{
    return glfwWindowShouldClose(window);
}

void View::closeWindow()
{
    for (map<string, util::ObjectInstance *>::iterator it = objects.begin();
         it != objects.end();
         it++)
    {
        it->second->cleanup();
        delete it->second;
    }
    glfwDestroyWindow(window);
    glfwTerminate();
}

void View::resetTrackBall()
{
    xRotAngle = 0;
    yRotAngle = 0;

    if (cameraMode == FPS)
    {
        left = 0.0f;
        up = 0.0f;
        personEyePosition = glm::vec3(0.0f, 25.0f, 0.0f);
    }
}

void View::setGlobal()
{
    cameraMode = GLOBAL;
}

void View::setChopper()
{
    cameraMode = CHOPPER;
}

void View::setFPS()
{
    cameraMode = FPS;
}

void View::moveForward()
{
    if (cameraMode == FPS)
    {
        personEyePosition += glm::vec3(0.0f, 0.0f, 1.0f) * 0.3f;
    }
}

void View::moveBackward()
{
    if (cameraMode == FPS)
    {
        personEyePosition -= glm::vec3(0.0f, 0.0f, 1.0f) * 0.3f;
    }
}

void View::turnUp()
{
    if (cameraMode == FPS)
    {
        up += 3.0f;
    }
}

void View::turnDown()
{
    if (cameraMode == FPS)
    {
        up -= 3.0f;
    }
}

void View::turnLeft()
{
    if (cameraMode == FPS)
    {
        // printf("\nturnLeft... left = %f", left);
        left -= 3.0f;
    }
}

void View::turnRight()
{
    if (cameraMode == FPS)
    {
        // printf("\nturnRight... left = %f", left);
        left += 3.0f;
    }
}

// document references: https://cgvr.cs.uni-bremen.de/teaching/cg_literatur/lighthouse3d_view_frustum_culling/
void View::setFrustumVertices()
{
    float fov = 80.0f;
    float nearDist = 0.1f;
    float farDist = 50.0f;

    // this could be in the middle
    float vertices[] = {
        0.0f, 0.0f, 0.0f, // starting point
        1.0f, 1.0f, 1.0f  // ending point
    };

    // unsigned int indices[] = { 0, 1 };

    vector<glm::vec4> positions;
    positions.push_back(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    positions.push_back(glm::vec4(0.5f, 0.0f, 0.0f, 1.0f));

    vector<VertexAttrib> vertexData;

    for (unsigned int i = 0; i < positions.size(); i++)
    {
        VertexAttrib v;
        vector<float> data;

        data.push_back(positions[i].x);
        data.push_back(positions[i].y);
        data.push_back(positions[i].z);
        data.push_back(positions[i].w);
        v.setData("position", data);

        vertexData.push_back(v);
    }

    vector<unsigned int> indices;

    for (int i = 0; i < positions.size(); i++)
    {
        indices.push_back(i);
    }

    util::PolygonMesh<VertexAttrib> mesh;
    mesh.setVertexData(vertexData);
    mesh.setPrimitives(indices);

    mesh.setPrimitiveType(
        GL_LINES);
    mesh.setPrimitiveSize(2);

    frustum_meshes.push_back(mesh);

    // if (cameraMode == GLOBAL) {
    //     personEyePosition = glm::vec3(100.0f, 120.0f, 150.0f);
    //     personDirection = glm::vec3(0.0f, 0.0f, 0.0f);
    // } else if (cameraMode == CHOPPER) {
    //     personEyePosition = glm::vec3(100.0f * sin(glm::radians(20.0f)), 200.0f, 100.0f * cos(glm::radians(20.0f)));
    //     personDirection = glm::vec3(50.0f, 100.0f, 10.0f);
    // }

    // float tanFOV = tan(glm::radians(fov / 2.0f));
    // // near
    // float nearHeight = 2.0f * tanFOV * nearDist;
    // float nearWidth = nearHeight * aspectRatio;

    // // far
    // float farHeight = 2.0f * tanFOV * farDist;
    // float farWidth = farHeight * aspectRatio;

    // glm::vec3 right = glm::normalize(glm::cross(personDirection, personUp));
    // glm::vec3 camera_up = glm::normalize(glm::cross(right, personDirection));

    // glm::vec3 fc = personEyePosition + personDirection * farDist;
    // glm::vec3 nc = personEyePosition + personDirection * nearDist;

    // glm::vec3 ftl = fc + (camera_up * farHeight/4.0f) + (right * farWidth/4.0f);
    // glm::vec3 ftr = fc + (camera_up * farHeight/4.0f) + (right * farWidth/4.0f);
    // glm::vec3 fbl = fc - (camera_up * farHeight/4.0f) - (right * farWidth/4.0f);
    // glm::vec3 fbr = fc - (camera_up * farHeight/4.0f) + (right * farWidth/4.0f);

    // glm::vec3 ntl = nc + (camera_up * nearHeight/4.0f) - (right * nearWidth/4.0f);
    // glm::vec3 ntr = nc + (camera_up * nearHeight/4.0f) + (right * nearWidth/4.0f);
    // glm::vec3 nbl = nc - (camera_up * nearHeight/4.0f) - (right * nearWidth/4.0f);
    // glm::vec3 nbr = nc - (camera_up * nearHeight/4.0f) + (right * nearWidth/4.0f);
}
