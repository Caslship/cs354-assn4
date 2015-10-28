/*
* Assignment #3
* Name: Jason Palacios
* UT EID: jap4839
* UTCS: jason777
*/

#ifndef __DEBUG_H__
#define __DEBUG_H__

#include <GL/glut.h>

static bool debug_flag = false;

GLfloat v[8][3] = {{-1.0, 1.0, 1.0}, {-1.0, -1.0, 1.0},
      {1.0, -1.0, 1.0}, {1.0, 1.0, 1.0},
      {1.0, 1.0, -1.0}, {1.0, -1.0, -1.0},
      {-1.0, -1.0, -1.0}, {-1.0, 1.0, -1.0}};

GLfloat center[3] = {0.0, 0.0, 0.0};

GLfloat colors[6][3] = {{1.0, 0.0, 0.0}, {0.0, 1.0, 0.0},
                        {0.0, 0.0, 1.0}, {1.0, 1.0, 1.0},
                        {1.0, 1.0, 0.0}, {1.0, 0.5, 0.0}};

void square(GLfloat * va, GLfloat * vb, GLfloat * vc, GLfloat * vd)
{
       glVertex3fv(va);
       glVertex3fv(vb);
       glVertex3fv(vc);
       glVertex3fv(vd);
}

void cube(void)
{
    glBegin(GL_QUADS);

        glColor3fv(colors[0]);
        square(v[0], v[1], v[2], v[3]);
        glColor3fv(colors[1]);
        square(v[2], v[3], v[4], v[5]);
        glColor3fv(colors[2]);
        square(v[0], v[1], v[6], v[7]);
        glColor3fv(colors[3]);
        square(v[7], v[6], v[5], v[4]);
        glColor3fv(colors[4]);
        square(v[0], v[3], v[4], v[7]);
        glColor3fv(colors[5]);
        square(v[1], v[2], v[5], v[6]);

    glEnd();
}

#endif