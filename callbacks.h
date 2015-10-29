/*
* Assignment #3
* Name: Jason Palacios
* UT EID: jap4839
* UTCS: jason777
*/

#ifndef __CALLBACKS_H__
#define __CALLBACKS_H__

#include <GL/glut.h>
#include <iostream>
#include <string>
#include <cctype>
#include <stack>
#include "geom.h"
#include "enums.h"
#include "debug.h"

using namespace std;

// Camera feature constants
#define VIEWING_DISTANCE_MIN  0.1
#define ZOOM_SPEED 5.0
#define ORBIT_SPEED 0.5
#define PAN_SPEED 0.01

/// Perspective variables
static int win_height = 500, g_height = 500;
static int win_width = 500, g_width = 500;
static float g_near_plane = 0.01;
static float g_far_plane = 10000.0;

/// Render
static GLenum render_mode = GL_FILL;
static NORM_FLAG_ID show_normal_flag = NO_NORM;
static stack<Trimesh> model_stack;

/// Transformations
static TRANS_FLAG_ID trans_flag = WORLD_COORDS;

/// Camera (zoom)
static float old_zoom_level;
static float zoom_level = 50.0 * VIEWING_DISTANCE_MIN;

/// Camera (orbit)
static int old_orbit_x, old_orbit_y;
static float orbit_theta = 0.0, orbit_phi = 0.0, orbit_delta = 0.0;

/// Camera (pan)
static int old_pan_x, old_pan_y;
static float pan_x = 0.0, pan_y = 0.0;

/// Camera (misc)
static bool orbit_camera_flag;
static bool zoom_camera_flag;
static bool pan_camera_flag;
static vertex_t look_at_pos;
extern void SetCameraView(void);

/// Lighting
static float g_light_pos[4] = {};
static float g_light_direction[4] = {};
extern void SetLighting(void);

/// CLI
static string command = "";
extern bool ExecuteCommand();

// Render 
void Display()
{
    // Clear screen and misc settings    
    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Start of with a fresh model view
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Set camera view
    SetCameraView();

    // Set light
    SetLighting();

    // Render the model
    if (!model_stack.empty())
    {
        model_stack.top().render(show_normal_flag, render_mode);
    }
    else if (debug_flag)
    {
        cube();
    }

    glutSwapBuffers();
}


// Update view port when resized
void Reshape(int w, int h)
{    
    g_width = win_width = w;
    g_height = win_height = h;

    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (float)g_width / (float)g_height, g_near_plane, g_far_plane);

    glMatrixMode(GL_MODELVIEW);

    glutPostRedisplay();
}

// Process mouse button clicks
void MouseButton(int button, int state, int x, int y)
{
    if (orbit_camera_flag = ((state == GLUT_DOWN) && (button == GLUT_LEFT_BUTTON)))
    {
        // Orbit
        old_orbit_x = x;
        old_orbit_y = y;
    }
    else if (zoom_camera_flag = ((state == GLUT_DOWN) && (button == GLUT_RIGHT_BUTTON)))
    {
        // Zoom
        old_zoom_level = y - (ZOOM_SPEED * zoom_level);
    }
    else if (pan_camera_flag = ((state == GLUT_DOWN) && (button == GLUT_MIDDLE_BUTTON)))
    {
        // Pan
        old_pan_x = x;
        old_pan_y = y;
    }
}

// Process mouse motion
void MouseMotion(int x, int y)
{
    if (orbit_camera_flag)
    {
        // Orbit
        orbit_theta += ((x - old_orbit_x) * ORBIT_SPEED);
        orbit_phi += ((y - old_orbit_y) * ORBIT_SPEED);

        old_orbit_x = x;
        old_orbit_y = y;

        glutPostRedisplay();
    }
    else if (zoom_camera_flag)
    {
        // Zoom
        zoom_level = (float)(y - old_zoom_level) / ZOOM_SPEED;
        if (zoom_level < VIEWING_DISTANCE_MIN)
            zoom_level = VIEWING_DISTANCE_MIN;

        glutPostRedisplay();
    }
    else if (pan_camera_flag)
    {
        // Pan
        pan_x += (x - old_pan_x) * PAN_SPEED;
        pan_y += (old_pan_y - y) * PAN_SPEED;

        old_pan_x = x;
        old_pan_y = y;

        glutPostRedisplay();
    }
}

// Process keyboard commands
void Keyboard(unsigned char key, int x, int y)
{
    if (key == 27)
    {
        // Escape (quit)

        exit(EXIT_SUCCESS);
    }

    // Ignore backspaces
    if (key != 8)
    {
        if (key == 13)
        {
            // Enter (execute command)

            cout << endl;

            if (!ExecuteCommand())
                cout << "Invalid command or parameters!" << endl;

            command.clear();

            glutPostRedisplay();
        }
        else
        {
            // Add to current key sequence
            
            cout << key;
            command += key;
        }
    }

    cout.flush();
}

#endif