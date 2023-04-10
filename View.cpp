#include "View.h"
#include <cstdio>
#include <cstdlib>
#include <vector>
using namespace std;
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "sgraph/GLScenegraphRenderer.h"
#include "sgraph/ScenegraphLightPosCalculator.h"
#include "raytracer/Ray.h"
#include "raytracer/HitRecord.h"
#include "sgraph/RayCaster.h"
#include "PPMImageExporter.h"
// #include "VertexAttrib.h"

#include "glm/gtx/string_cast.hpp" // added just for printing

View::View()
{
}

View::~View()
{
}

void View::init(Callbacks *callbacks, Model &model)
{
    raytraceEnabled = false;
    sgraph::IScenegraph *scenegraph = model.getScenegraph();
    map<string, util::PolygonMesh<VertexAttrib>> meshes = scenegraph->getMeshes();

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

    program.createProgram(string("shaders/phong-multiple-with-texture-spotlights.vert"), string("shaders/phong-multiple-with-texture-spotlights.frag"));

    //  enable the shader program
    program.enable();
    shaderLocations = program.getAllShaderVariables();

    map<string, string> shaderVarsToVertexAttribs;

    shaderVarsToVertexAttribs["vPosition"] = "position";
    shaderVarsToVertexAttribs["vNormal"] = "normal";
    shaderVarsToVertexAttribs["vTexCoord"] = "texcoord";

    for (typename map<string, util::PolygonMesh<VertexAttrib>>::iterator it = meshes.begin();
         it != meshes.end();
         it++)
    {
        util::ObjectInstance *obj = new util::ObjectInstance(it->first);
        obj->initPolygonMesh(program,
                             shaderLocations,
                             shaderVarsToVertexAttribs,
                             it->second);
        objects[it->first] = obj;
    }

    printf("before renderer initialization\n");
    renderer = new sgraph::GLScenegraphRenderer(modelview, objects, shaderLocations);
    printf("after renderer initialization\n");

    printf("before lightPosCalculator initialization\n");
    lightPosCalculator = new sgraph::ScenegraphLightPosCalculator(modelview);
    printf("after lightPosCalculator initialization\n");

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

void View::initLightShaderVars()
{
    // get input variables that need to be given to the shader program
    for (int i = 0; i < lights.size(); i++)
    {
        LightLocation ll;
        stringstream name;

        name << "light[" << i << "]";
        ll.ambient = shaderLocations.getLocation(name.str() + "" + ".ambient");
        ll.diffuse = shaderLocations.getLocation(name.str() + ".diffuse");
        ll.specular = shaderLocations.getLocation(name.str() + ".specular");
        ll.position = shaderLocations.getLocation(name.str() + ".position");
        ll.spotDirection = shaderLocations.getLocation(name.str() + ".spotDirection");
        ll.spotAngle = shaderLocations.getLocation(name.str() + ".spotAngle");
        ll.isSpotlight = shaderLocations.getLocation(name.str() + ".isSpotlight");
        lightLocations.push_back(ll);
    }
}

void View::raytrace(sgraph::IScenegraph *scenegraph, int w, int h, stack<glm::mat4> modelview)
{
    /**
     * populates the image of the given dimensions with the output to ray tracing and write the image to a PPM image
     * (look at how a PPM file is imported for a texture, and write the image similarly).
     * The modelview stack currently contains the camera transform that is set in the View::draw method.
     *
     * For every pixel:
     * Compute the ray starting from the eye and passing through this pixel in view coordinates.
     * Pass the ray to the ray caster renderer (below) that returns information about ray casting.
     * Write the color to the appropriate place in the array.
     */

    // calculate intersection (t)
    // Q: where is the origin of the ray coming from? -> camera origin
    // create the 3DRay object with origin and t
    // pass the 3DRay to RayCaster
    // RayCaster returns color
    // write the returned color to array

    // output image buffer
    vector<glm::vec3> imageColors;

    float imageAspectRatio = (float)w / h;
    float fov = 100.0f;

    // for every pixel (i,j)
    for (int i = 0; i < w; i++)
    {
        for (int j = 0; j < h; j++)
        {
            // calculate ray direction w.r.t. current pixel (i,j)
            glm::vec4 rayDirection = glm::vec4(i - 0.5 * w, j - 0.5 * h, -(0.5 * h) / tan(glm::radians(fov / 2)), 0);
            glm::vec4 rayOrigin = glm::vec4(0, 0, 0, 1);
            rayDirection = normalize(rayDirection);

            raytracer::Ray ray = raytracer::Ray(rayOrigin, rayDirection);

            // initialize raycaster for this ray
            sgraph::RayCaster *raycaster = new sgraph::RayCaster(modelview, ray, objects);

            // raycaster descends down the scenegraph to find the closest HitRecord
            scenegraph->getRoot()->accept(raycaster);

            // based on all HitRecords encountered (or none), raycaster calculates the color for this pixel
            glm::vec3 pixelColor = raycaster->getPixelColor(); // TODO:

            // add pixelColor to vector of all pixel colors
            imageColors.push_back(pixelColor);
        }
    }

    // export vector of image pixel colors into PPM image
    PPMImageExporter exporter = PPMImageExporter();
    cout << "meowww" << imageColors[0] << endl
         << endl;
    exporter.exportToPPM(w, h, imageColors);
}

void View::display(sgraph::IScenegraph *scenegraph)
{
    printf("in view display now\n");
    program.enable();
    // glClearColor(0, 0, 0, 1);
    glClearColor(1, 1, 1, 1); // TODO: probably change this while testing raytracing
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    modelview.push(glm::mat4(1.0));

    if (cameraMode == GLOBAL)
    {
        modelview.top() = modelview.top() *
                          // glm::lookAt(glm::vec3(100.0f, 100.0f, 150.0f),
                          glm::lookAt(glm::vec3(0.0f, 0.0f, 100.0f),
                                      // glm::lookAt(glm::vec3(0.0f, 0.0f, 150.0f),
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
        modelview.top() = modelview.top() * glm::lookAt(personEyePosition, personDirection, personUp);
        modelview.top() = modelview.top() *
                          glm::rotate(glm::mat4(1.0), glm::radians(left), glm::vec3(0.0f, 1.0f, 0.0f)) *
                          glm::rotate(glm::mat4(1.0), glm::radians(up), glm::vec3(1.0f, 0.0f, 0.0f));
    }

    modelview.push(modelview.top());
    modelview.top() = modelview.top() *
                      glm::rotate(glm::mat4(1.0), glm::radians(yRotAngle), glm::vec3(1.0f, 0.0f, 0.0f)) *
                      glm::rotate(glm::mat4(1.0), glm::radians(xRotAngle), glm::vec3(0.0f, 1.0f, 0.0f));

    glUniformMatrix4fv(shaderLocations.getLocation("projection"), 1, GL_FALSE, glm::value_ptr(projection));

    if (raytraceEnabled)
    {
        int window_width, window_height;
        glfwGetFramebufferSize(window, &window_width, &window_height);
        cout << "in view.display, about to call raytrace w/ modelview.top(): " << modelview.top() << endl;
        raytrace(scenegraph, 300, 300, modelview); // TODO: Q: are the width and height the window width & height?
    }

    // LIGHTS
    // calculate light positions & spotlight directions
    printf("before light pos calculations\n");
    scenegraph->getRoot()->accept(lightPosCalculator);
    printf("after light pos calculations\n");
    lights = lightPosCalculator->getScenegraphLights();
    printf("now there are %i lights in View\n", lights.size());

    // get input variables that need to be given to the shader program
    initLightShaderVars();

    // draw lights
    for (int i = 0; i < lights.size(); i++)
    {
        printf("\ngot here\n");
        glm::vec4 pos = lights[i].getPosition();
        cout << "fetched light pos: " << pos << endl;
        cout << lightLocations.size() << endl;

        glUniform4fv(lightLocations[i].position, 1, glm::value_ptr(pos));
    }
    printf("\nalso got here\n");

    // pass light color properties to shader
    glUniform1i(shaderLocations.getLocation("numLights"), lights.size());

    // pass light colors to the shader
    for (int i = 0; i < lights.size(); i++)
    {
        cout << "isSpotlight: " << lights[i].getIsSpotlight() << endl;
        glUniform3fv(lightLocations[i].ambient, 1, glm::value_ptr(lights[i].getAmbient()));
        glUniform3fv(lightLocations[i].diffuse, 1, glm::value_ptr(lights[i].getDiffuse()));
        glUniform3fv(lightLocations[i].specular, 1, glm::value_ptr(lights[i].getSpecular()));
        glUniform3fv(lightLocations[i].spotDirection, 1, glm::value_ptr(lights[i].getSpotDirection()));
        glUniform1f(lightLocations[i].spotAngle, cos(glm::radians(lights[i].getSpotCutoff()))); // sending cosine of angle to shader
        glUniform1i(lightLocations[i].isSpotlight, lights[i].getIsSpotlight());
    }

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
        frames = 0;
        time = currenttime;
    }

    lightLocations.clear();
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
        // // printf("\nturnLeft... left = %f", left);
        left -= 3.0f;
    }
}

void View::turnRight()
{
    if (cameraMode == FPS)
    {
        // // printf("\nturnRight... left = %f", left);
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
}

void View::toggleRaytraceMode()
{
    raytraceEnabled = !raytraceEnabled;
}
