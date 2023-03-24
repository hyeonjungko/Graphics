//! [code]

#include <glad/glad.h>
#include "View.h"
#include "Model.h"
#include "Controller.h"

int main(int argc,char *argv[]) {
    string file_path;
    Model model;
    View view;

     // Scenegraphs.exe -i file_path
    for (int i = 0; i < argc; ++i) {
        if (i == 2) { 
            file_path = argv[i];
        }
    }

    Controller controller(model,view, file_path);
    controller.run();


}

//! [code]
