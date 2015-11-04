/*
* Assignment #4
* Name: Jason Palacios
* UT EID: jap4839
* UTCS: jason777
*/

#ifndef __CALLBACKS_H__
#define __CALLBACKS_H__

#include <GL/glut.h>
#include <GL/glui.h>
#include <string>
#include <cctype>
#include <iostream>
#include "geom.h"
#include "enums.h"
#include "scenegraph.h"
#include "node.h"

using namespace std;

// Camera feature constants
#define VIEWING_DISTANCE_MIN  0.1
#define ZOOM_SPEED 5.0
#define ORBIT_SPEED 0.5
#define PAN_SPEED 0.01

/// Perspective variables
static int g_height = 600;
static int g_width = 800;
static float g_near_plane = 0.01;
static float g_far_plane = 10000.0;

/// Scene graph
static SceneGraphContainer scenegraph;

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
static int vx = 0;
static int vy = 0;
static vertex_t look_at_pos;

/// Lighting
static float g_light_pos[4] = {};
static float g_light_direction[4] = {};

/// CLI
static string command = "";
extern bool ExecuteCommand();

/// GUI
int main_window;
GLUI * glui;
GLUI_Panel * scene_graph_panel;
GLUI_Panel * select_node_panel;
GLUI_Listbox * child_node_select;
GLUI_Button * select_child_node;
GLUI_Button * select_parent_node;
GLUI_Panel * add_node_panel;
GLUI_Listbox * node_type_select;
GLUI_Button * add_child_node;
GLUI_Button * add_parent_node;
GLUI_Panel * curr_node_panel;
GLUI_Panel * attr_node_panel;
GLUI_Listbox * render_type_select;
GLUI_Panel * geom_node_panel;
GLUI_EditText * geom_path;
GLUI_Panel * transform_node_panel;
GLUI_Listbox * transform_type_select;
GLUI_Listbox * transform_coord_type_select;
GLUI_EditText * x_param;
GLUI_EditText * y_param;
GLUI_EditText * z_param;
GLUI_EditText * theta_param;
GLUI_Panel * camera_node_panel;
GLUI_EditText * vx_param;
GLUI_EditText * vy_param;
GLUI_Button * update_node;
GLUI_Button * delete_node;

int child_node_index = 0;
int node_type_index = 0;
int render_type_index = 0;
int transform_type_index = 0;
int transform_coord_type_index = 0;

string render_type_list[] = { "Points", "Wireframe", "Solid", "Shaded", "Face Normals", "Vertex Normals" };
string node_type_list[] = { "Object", "Geometry", "Transform", "Attribute", "Light" };
string transform_type_list[] = { "Scale", "Translate", "Rotate" };
string transform_coord_type_list[] = { "World", "View" };

extern void UpdateGUI(int);

// Render 
void Display(void)
{
    // Clear screen and misc settings    
    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Start of with a fresh model view
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Traverse the scene graph
    scenegraph.traverseGraph();

    glutSwapBuffers();
}


// Update view port when resized
void Reshape(int w, int h)
{    
    g_width = w;
    g_height = h;

    glViewport(vx, vy, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (float)g_width / (float)g_height, g_near_plane, g_far_plane);

    glMatrixMode(GL_MODELVIEW);

    glutPostRedisplay();
}

// Process mouse button clicks
void MouseButton(int button, int state, int x, int y)
{
    if ((orbit_camera_flag = ((state == GLUT_DOWN) && (button == GLUT_LEFT_BUTTON))))
    {
        // Orbit
        old_orbit_x = x;
        old_orbit_y = y;
    }
    else if ((zoom_camera_flag = ((state == GLUT_DOWN) && (button == GLUT_RIGHT_BUTTON))))
    {
        // Zoom
        old_zoom_level = y - (ZOOM_SPEED * zoom_level);
    }
    else if ((pan_camera_flag = ((state == GLUT_DOWN) && (button == GLUT_MIDDLE_BUTTON))))
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
}

void Idle(void)
{
    if (glutGetWindow() != main_window) 
        glutSetWindow(main_window);   
}

void Control(int control_id)
{
    Node * curr_node = scenegraph.getCurrentNode();
    int old_children_vec_size = curr_node->getChildCount();

    switch(control_id)
    {
        case CHILD_NODE_SELECT_B_ID:
        {
            Node * child_node = curr_node->getChild(child_node_index);

            if (child_node != NULL)
            {
                scenegraph.setCurrentNode(child_node);
            }

            break;
        }
        case PARENT_NODE_SELECT_B_ID:
        {
            Node * parent_node = curr_node->getParent();

            if (parent_node != NULL)
                scenegraph.setCurrentNode(parent_node);

            break;
        }
        case NODE_TYPE_LB_ID:
        case TRANSFORM_TYPE_LB_ID:
        {
            break;
        }
        case CHILD_NODE_ADD_B_ID:
        {
            switch(node_type_index)
            {
                case 0:
                {
                    ObjectNode * object_node = new ObjectNode(curr_node);
                    break;
                }
                case 1:
                {
                    GeometryNode * geom_node = new GeometryNode(curr_node);
                    break;
                }
                case 2:
                {
                    TransformNode * transform_node = new TransformNode(curr_node);
                    break;
                }
                case 3:
                {
                    AttributeNode * attr_node = new AttributeNode(curr_node);
                    break;
                }
                case 4:
                {
                    LightNode * light_node = new LightNode(curr_node);
                    break;
                }
            }
            break;
        }
        case PARENT_NODE_ADD_B_ID:
        {
            switch(node_type_index)
            {
                case 0:
                {
                    ObjectNode * object_node = new ObjectNode();
                    curr_node->addParent(object_node);
                    break;
                }
                case 1:
                {
                    GeometryNode * geom_node = new GeometryNode();
                    curr_node->addParent(geom_node);
                    break;
                }
                case 2:
                {
                    TransformNode * transform_node = new TransformNode();
                    curr_node->addParent(transform_node);
                    break;
                }
                case 3:
                {
                    AttributeNode * attr_node = new AttributeNode();
                    curr_node->addParent(attr_node);
                    break;
                }
                case 4:
                {
                    LightNode * light_node = new LightNode();
                    curr_node->addParent(light_node);
                    break;
                }
            }
            break;
        }
        case CURR_NODE_UPDATE_B_ID:
        {
            string curr_node_type = curr_node->getNodeType();

            if (curr_node_type == "Attribute")
            {
                string render_type = render_type_list[render_type_index];
                ((AttributeNode *)curr_node)->setParams(render_type);
            }
            else if (curr_node_type == "Geometry")
            {
                ((GeometryNode *)curr_node)->setMesh(geom_path->get_text());
            }
            else if (curr_node_type == "Transform")
            {
                string transform_type = transform_type_list[transform_type_index];
                float xyz[3] = { x_param->get_float_val(), y_param->get_float_val(), z_param->get_float_val() };
                float theta = theta_param->get_float_val();

                ((TransformNode *)curr_node)->setParams(transform_type, xyz, theta);
            }
            else if (curr_node_type == "Camera")
            {
                int new_vx = vx_param->get_int_val();
                int new_vy = vy_param->get_int_val();

                if ((new_vx >= 0 && new_vx <= g_width) && (new_vy >= 0 && new_vy <= g_height))
                {
                    vx = new_vx;
                    vy = new_vy;

                    Reshape(g_width, g_height);
                }
            }
            break;
        }
        case CURR_NODE_DELETE_B_ID:
        {
            break;
        }
    }

    UpdateGUI(old_children_vec_size);
    glutPostRedisplay();
}

#endif
