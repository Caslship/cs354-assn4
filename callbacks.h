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

/// Perspective variables
static int g_height = 600;
static int g_width = 800;
static float g_near_plane = 0.01;
static float g_far_plane = 10000.0;

/// Scene graph
static SceneGraphContainer scenegraph;

/// GUI (widgets)
int main_window;
GLUI * glui;
GLUI_Panel * scene_graph_panel;
GLUI_Rollout * select_node_panel;
GLUI_Listbox * child_node_select;
GLUI_Button * select_child_node;
GLUI_Button * select_parent_node;
GLUI_Rollout * add_node_panel;
GLUI_Listbox * node_type_select;
GLUI_Button * add_child_node;
GLUI_Button * add_parent_node;
GLUI_Panel * curr_node_panel;
GLUI_Rollout * attr_node_panel;
GLUI_Listbox * render_type_select;
GLUI_Rollout * geom_node_panel;
GLUI_EditText * geom_path;
GLUI_Rollout * transform_node_panel;
GLUI_Listbox * transform_type_select;
// GLUI_Listbox * transform_coord_type_select;
GLUI_EditText * x_param;
GLUI_EditText * y_param;
GLUI_EditText * z_param;
GLUI_EditText * theta_param;
GLUI_Rollout * camera_node_panel;
GLUI_EditText * vx_param;
GLUI_EditText * vy_param;
GLUI_Rollout * light_node_panel;
GLUI_Listbox * light_type_select;
GLUI_Button * update_node;
GLUI_Button * delete_node;

/// GUI (live variables)
int child_node_index = 0;
int node_type_index = 0;
int render_type_index = 0;
int transform_type_index = 0;
int transform_coord_type_index = 0;
int light_type_index = 0;

/// GUI (listbox and other constant contents)
string render_type_list[] = { "Points", "Wireframe", "Solid", "Shaded", "Face Normals", "Vertex Normals" };
string node_type_list[] = { "Object", "Geometry", "Transform", "Attribute", "Light" };
string transform_type_list[] = { "Scale", "Translate", "Rotate" };
// string transform_coord_type_list[] = { "World", "View" };
string light_type_list[] = { "Point", "Directional" };
const GLenum light_ids[] = { GL_LIGHT0, GL_LIGHT1, GL_LIGHT2, GL_LIGHT3, GL_LIGHT4, GL_LIGHT5, GL_LIGHT6, GL_LIGHT7 };

extern void UpdateGUI(int);

// Render 
void Display(void)
{
    // Clear screen  
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Traverse the scene graph
    scenegraph.traverseGraph();

    glutSwapBuffers();
}


// Update view port when resized
void Reshape(int w, int h)
{    
    g_width = w;
    g_height = h;

    CameraNode * camera_node = scenegraph.getCameraNode();
    int vx = camera_node->getViewportX();
    int vy = camera_node->getViewportY();



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
    (scenegraph.getCameraNode())->processMouseButton(button, state, x, y);
}

// Process mouse motion
void MouseMotion(int x, int y)
{
    (scenegraph.getCameraNode())->processMouseMotion(x, y);
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
                scenegraph.setCurrentNode(child_node);

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
            // The only thing we need is for the GUI to update
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
                    if (scenegraph.incLightCount())
                        LightNode * light_node = new LightNode(light_ids[scenegraph.getLightCount() - 1], curr_node);

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
                    if (scenegraph.incLightCount())
                    {
                        LightNode * light_node = new LightNode(light_ids[scenegraph.getLightCount() - 1], NULL);
                        curr_node->addParent(light_node);
                    }
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
                int vx = vx_param->get_int_val();
                int vy = vy_param->get_int_val();

                if ((vx >= 0 && vx <= g_width) && (vy >= 0 && vy <= g_height))
                {
                    ((CameraNode *)curr_node)->setViewportXY(vx, vy);

                    Reshape(g_width, g_height);
                }
            }
            else if (curr_node_type == "Light")
            {
                string light_type = light_type_list[light_type_index];

                ((LightNode *)curr_node)->setType(light_type);
            }
            break;
        }
        case CURR_NODE_DELETE_B_ID:
        {
            Node * parent = curr_node->getParent();
            string curr_node_type  = curr_node->getNodeType();

            if (parent != NULL && curr_node != scenegraph.getRootNode() && curr_node_type != "Camera")
            {
                if (curr_node_type == "Light" && !scenegraph.decLightCount())
                    break;

                curr_node->removeNode();
                scenegraph.setCurrentNode(parent);
            }
            break;
        }
    }

    UpdateGUI(old_children_vec_size);
    glutPostRedisplay();
}

#endif
