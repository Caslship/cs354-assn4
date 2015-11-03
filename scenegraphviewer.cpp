/*
* Assignment #3
* Name: Jason Palacios
* UT EID: jap4839
* UTCS: jason777
*/

#include <GL/glut.h>
#include "helpers.h"

int main(int argc, char ** argv)
{  
    // I like small main functions  
    InitializeWindow(argc, argv);
    InitializeGraphics();
    RegisterCallbacks();
    InitializeGUI();

    glutMainLoop();
    return 0;
}
