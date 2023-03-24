#ifndef __MODEL_H__
#define __MODEL_H__

#include "PolygonMesh.h"
#include "VertexAttrib.h"
#include "sgraph/IScenegraph.h"
#include <map>
using namespace std;

class Model
{
public:
    Model();
    ~Model();
    sgraph::IScenegraph *getScenegraph();
    void setScenegraph(sgraph::IScenegraph *scenegraph);
    bool getIsLeftMousePressed();
    void leftMouseReleased();
    void leftMousePressed();
    void setMousePositions(tuple<float, float> mousePositions);
    tuple<float, float> calculateCursorDelta(double xpos, double ypos);
    tuple<float, float> calculateRotationAngles(double xpos, double ypos);

private:
    sgraph::IScenegraph *scenegraph;
    bool isLeftMousePressed;
    float prev_mouse_x = 0;
    float prev_mouse_y = 0;

};
#endif