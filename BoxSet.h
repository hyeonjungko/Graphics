#ifndef __BOXSET_H__
#define __BOXSET_H__

#include "Box.h"
#include <vector>
#include <iostream>
using namespace std;
#include "Material.h"

/**
 * @brief This class represents a set of non-overlapping boxes.
 *
 */
class BoxSet
{

public:
    BoxSet();

    ~BoxSet();

    void add(const Box &b);
    void difference(const Box &other);
    void toScenegraph(ostream &out, util::Material &mat);
    void toScenegraph(ostream &out);

private:
    BoxSet(vector<Box> &boxes);
    vector<Box> boxes;
};

BoxSet::BoxSet()
{
}

BoxSet::BoxSet(vector<Box> &boxes)
{
    this->boxes = boxes;
}

BoxSet::~BoxSet()
{
}

void BoxSet::add(const Box &b)
{
    vector<Box> result;
    this->difference(b);

    // add b to result
    boxes.push_back(b);
}

void BoxSet::difference(const Box &other)
{
    vector<Box> result;

    if (this->boxes.size() == 0)
    {
        return; // empty
    }

    // (a + b + c) \ d = (a\d) + (b\d) + (c\d)
    // a\d = a \\ a*d  where \\ is containedDifference and * is intersect
    for (int i = 0; i < boxes.size(); i++)
    {
        if (other.overlaps(boxes[i]))
        {
            Box common = boxes[i].intersect(other);
            vector<Box> containedResult = boxes[i].containedDifference(common);
            result.insert(result.end(), containedResult.begin(), containedResult.end());
        }
        else
        {
            result.push_back(boxes[i]);
        }
    }
    boxes = result;
}

void BoxSet::toScenegraph(ostream &out, util::Material &mat)
{
    out << "#all the instances" << endl;
    out << "instance box models/box.obj" << endl;

    out << "#material" << endl;
    out << "material mat" << endl;
    out << "emission " << mat.getEmission()[0] << " "
        << mat.getEmission()[1] << " "
        << mat.getEmission()[2] << endl;

    out << "ambient " << mat.getAmbient()[0] << " "
        << mat.getAmbient()[1] << " "
        << mat.getAmbient()[2] << endl;

    out << "diffuse " << mat.getDiffuse()[0] << " "
        << mat.getDiffuse()[1] << " "
        << mat.getDiffuse()[2] << endl;

    out << "specular " << mat.getSpecular()[0] << " "
        << mat.getSpecular()[1] << " "
        << mat.getSpecular()[2] << endl;

    out << "shininess " << mat.getShininess() << endl;
    out << "end-material" << endl;

    out << "group root root" << endl;
    out << "assign-root root" << endl;

    for (int i = 0; i < boxes.size(); i++)
    {
        float tx, ty, tz;
        float sx, sy, sz;

        tx = boxes[i].getX() + boxes[i].getWidth() / 2;
        ty = boxes[i].getY() + boxes[i].getHeight() / 2;
        tz = boxes[i].getZ() + boxes[i].getDepth() / 2;
        sx = boxes[i].getWidth();
        sy = boxes[i].getHeight();
        sz = boxes[i].getDepth();
        out << "translate trans-" << i << " trans-" << i << " "
            << tx << " " << ty << " " << tz << endl;
        out << "add-child trans-" << i << " root" << endl;

        out << "scale scale-" << i << " scale-" << i << " "
            << sx << " " << sy << " " << sz << endl;
        out << "add-child scale-" << i << " trans-" << i << endl;

        out << "leaf box-" << i << " box-" << i << " instanceof box" << endl;
        out << "assign-material box-" << i << " mat" << endl;
        out << "add-child box-" << i << " scale-" << i << endl;
    }
}

void BoxSet::toScenegraph(ostream &out)
{
    out << "#all the instances" << endl;
    out << "instance box models/box.obj" << endl;

    for (int i = 0; i < boxes.size(); i++)
    {
        float red, green, blue;
        red = (float)rand() / RAND_MAX;
        green = (float)rand() / RAND_MAX;
        blue = (float)rand() / RAND_MAX;

        out << "material mat-" << i << endl;
        out << "emission " << red << " "
            << green << " "
            << blue << endl;

        out << "ambient " << red << " "
            << green << " "
            << blue << endl;

        out << "diffuse " << red << " "
            << green << " "
            << blue << endl;

        out << "specular " << red << " "
            << green << " "
            << blue << endl;

        out << "shininess 10" << endl;
        out << "end-material" << endl;
    }

    out << "group root root" << endl;
    out << "assign-root root" << endl;

    for (int i = 0; i < boxes.size(); i++)
    {
        float tx, ty, tz;
        float sx, sy, sz;

        tx = boxes[i].getX() + boxes[i].getWidth() / 2;
        ty = boxes[i].getY() + boxes[i].getHeight() / 2;
        tz = boxes[i].getZ() + boxes[i].getDepth() / 2;
        sx = boxes[i].getWidth();
        sy = boxes[i].getHeight();
        sz = boxes[i].getDepth();
        out << "translate trans-" << i << " trans-" << i << " "
            << tx << " " << ty << " " << tz << endl;
        out << "add-child trans-" << i << " root" << endl;

        out << "scale scale-" << i << " scale-" << i << " "
            << sx << " " << sy << " " << sz << endl;
        out << "add-child scale-" << i << " trans-" << i << endl;

        out << "leaf box-" << i << " box-" << i << " instanceof box" << endl;
        out << "assign-material box-" << i << " mat-" << i << endl;
        out << "add-child box-" << i << " scale-" << i << endl;
    }
}

#endif