#include "Model.h"
#include <GLFW/glfw3.h>

Model::Model()
{
    isLeftMousePressed = false;
    prev_mouse_x, prev_mouse_y = 0;
}

Model::~Model()
{
    if (scenegraph)
    {
        delete scenegraph;
    }
}

bool Model::getIsLeftMousePressed()
{
    return isLeftMousePressed;
}

void Model::leftMouseReleased()
{
    isLeftMousePressed = false;
}

void Model::leftMousePressed()
{
    isLeftMousePressed = true;
}

void Model::setMousePositions(tuple<float, float> mousePositions)
{
    prev_mouse_x = get<0>(mousePositions);
    prev_mouse_y = get<1>(mousePositions);
}

tuple<float, float> Model::calculateCursorDelta(double xpos, double ypos)
{
    // calculate delta
    float delta_x = (float)xpos - prev_mouse_x;
    float delta_y = (float)ypos - prev_mouse_y;

    return make_tuple(delta_x, delta_y);
}

tuple<float, float> Model::calculateRotationAngles(double xpos, double ypos)
{
    tuple<float, float> deltas = calculateCursorDelta(xpos, ypos);
    float rot_x = (float)(0.5f * get<0>(deltas));
    float rot_y = (float)(0.5f * get<1>(deltas));

    return make_tuple(rot_x, rot_y);
}

sgraph::IScenegraph *Model::getScenegraph()
{
    return this->scenegraph;
}

void Model::setScenegraph(sgraph::IScenegraph *scenegraph)
{
    this->scenegraph = scenegraph;
}
