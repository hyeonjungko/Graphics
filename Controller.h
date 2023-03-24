#ifndef __CONTROLLER_H__
#define __CONTROLLER_H__

#include "View.h"
#include "Model.h"
#include "Callbacks.h"

class Controller : public Callbacks
{
public:
    Controller(Model &m, View &v, string &file_path);
    ~Controller();
    void run();

    virtual void reshape(int width, int height);
    virtual void dispose();
    virtual void onkey(int key, int scancode, int action, int mods);
    virtual void onmouse(int button, int action, int mods);
    virtual void oncursorpos(double xpos, double ypos);
    virtual void error_callback(int error, const char *description);
    void setPreviousMousePos(float xpos, float ypos);
    tuple<float, float> getPreviousMousePos();

private:
    void initScenegraph();

    View view;
    Model model;
    string file_path;
};

#endif