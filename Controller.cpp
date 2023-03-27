#include "Controller.h"
#include "sgraph/IScenegraph.h"
#include "sgraph/Scenegraph.h"
#include "sgraph/GroupNode.h"
#include "sgraph/LeafNode.h"
#include "sgraph/ScaleTransform.h"
#include "ObjImporter.h"
using namespace sgraph;
#include <iostream>
#include <tuple>
using namespace std;

#include "sgraph/ScenegraphExporter.h"
#include "sgraph/ScenegraphImporter.h"
#include "sgraph/SceneGraphTextRender.h"

Controller::Controller(Model &m, View &v, string &fp)
{
    model = m;
    view = v;
    file_path = fp;

    initScenegraph();
}

void Controller::initScenegraph()
{

    // read in the file of commands
    ifstream inFile(file_path);

    sgraph::ScenegraphImporter importer;
    sgraph::SceneGraphTextRender textrender;

    cout << "scenegraph: "
         << "\n";
    textrender.render(inFile, 1, "");

    inFile.clear();
    inFile.seekg(0, inFile.beg);

    cout << "----------------------"
         << "\n";

    IScenegraph *scenegraph = importer.parse(inFile);
    model.setScenegraph(scenegraph);
}

Controller::~Controller()
{
}

void Controller::run()
{
    sgraph::IScenegraph *scenegraph = model.getScenegraph();
    // map<string, util::PolygonMesh<VertexAttrib>> meshes = scenegraph->getMeshes();
    // view.init(this, meshes);
    view.init(this, model);

    while (!view.shouldWindowClose())
    {
        view.display(scenegraph);
    }
    view.closeWindow();

    exit(EXIT_SUCCESS);
}

void Controller::onkey(int key, int scancode, int action, int mods)
{

    if (key == GLFW_KEY_R && action == GLFW_PRESS)
    {
        view.resetTrackBall();
    }
    else if (key == GLFW_KEY_1 && action == GLFW_PRESS)
    {
        cout << "global pressed" << endl;
        view.setGlobal();
    }
    else if (key == GLFW_KEY_2 && action == GLFW_PRESS)
    {
        cout << "chopper pressed" << endl;
        view.setChopper();
    }
    else if (key == GLFW_KEY_3 && action == GLFW_PRESS)
    {
        cout << "fps pressed" << endl;
        view.setFPS();
        view.resetTrackBall();
    }
    else if (key == GLFW_KEY_KP_ADD || ((key == GLFW_KEY_EQUAL) && ((mods & GLFW_MOD_SHIFT != 0))))
    {
        view.moveForward();
    }
    else if (key == GLFW_KEY_KP_SUBTRACT || key == GLFW_KEY_MINUS)
    {
        view.moveBackward();
    }
    else if (key == GLFW_KEY_LEFT)
    {
        view.turnLeft();
    }
    else if (key == GLFW_KEY_RIGHT)
    {
        view.turnRight();
    }
    else if (key == GLFW_KEY_UP)
    {
        view.turnUp();
    }
    else if (key == GLFW_KEY_DOWN)
    {
        view.turnDown();
    }
}

void Controller::onmouse(int button, int action, int mods)
{

    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {

        if (action == GLFW_RELEASE)
        {

            if (model.getIsLeftMousePressed())
            {
                model.leftMouseReleased();
            }
            //  TODO: may have to do additional logic in model to stop rotation once mouse is released
        }
        else if (action == GLFW_PRESS)
        {
            if (!model.getIsLeftMousePressed())
            {
                model.leftMousePressed();

                tuple<float, float> currMousePos = view.getCurrentMousePositions();

                // tuple<float, float> currMousePos = ;
                model.setMousePositions(currMousePos);

                /**
                 * when the mouse get clicked but not dragging, rotAng should not change
                 * when dragging
                 */
            }
        }
    }
}

void Controller::oncursorpos(double xpos, double ypos)
{
    // call model's calculate cursor delta
    if (model.getIsLeftMousePressed())
    {
        tuple<float, float> rotAngles = model.calculateRotationAngles(xpos, ypos);
        //  once model returns the cursor delta, call view's updateTrackBallRotation(float xdelta, ydelta)

        cout << "angle " << get<0>(rotAngles) << " " << get<1>(rotAngles) << endl;

        view.updateTrackBallRotation(get<0>(rotAngles), get<1>(rotAngles));
    }
}

void Controller::reshape(int width, int height)
{

    glViewport(0, 0, width, height);
    view.setProjection(width, height);
}

void Controller::dispose()
{
    view.closeWindow();
}

void Controller::error_callback(int error, const char *description)
{
    fprintf(stderr, "Error: %s\n", description);
}