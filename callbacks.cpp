/*
* Assignment #4
* Name: Jason Palacios
* UT EID: jap4839
* UTCS: jason777
*/

#include "callbacks.h"

#include <GL/glut.h>
#include <GL/glui.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include "enums.h"
#include "helpers.h"
#include "scenegraph.h"
#include "node.h"
#include "cameranode.h"
#include "lightnode.h"
#include "geometrynode.h"
#include "objectnode.h"
#include "transformnode.h"
#include "attributenode.h"
#include <iostream>

using namespace std;

/// Perspective variables
int g_height = 600;
int g_width = 800;
float g_near_plane = 0.01;
float g_far_plane = 10000.0;

/// Scene graph
SceneGraphContainer scenegraph;

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
GLUI_EditText * x_param;
GLUI_EditText * y_param;
GLUI_EditText * z_param;
GLUI_EditText * theta_param;
GLUI_Checkbox * animation_param;
GLUI_Rollout * camera_node_panel;
GLUI_EditText * fov_param;
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
int light_type_index = 0;

/// GUI (listbox and other constant contents)
string render_type_list[] = { "Points", "Wireframe", "Solid", "Shaded", "Face Normals", "Vertex Normals" };
string node_type_list[] = { "Object", "Geometry", "Transform", "Attribute", "Light" };
string transform_type_list[] = { "Scale", "Translate", "Rotate" };
string light_type_list[] = { "Point", "Directional" };

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

    // Grab camera node fov, vx, and vy parameters
    CameraNode * camera_node = scenegraph.getCameraNode();
    float fov = camera_node->getFOV();
    int vx = camera_node->getViewportX();
    int vy = camera_node->getViewportY();

    // Set viewport
    glViewport(vx, vy, w, h);

    // Create projection matrix
    glm::mat4 projection_mat = glm::perspective(glm::radians(fov), (float)g_width / (float)g_height, g_near_plane, g_far_plane);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glMultMatrixf(glm::value_ptr(projection_mat));

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

// Make sure to set the main window (we do this because of GLUI)
void Idle(void)
{
    if (glutGetWindow() != main_window) 
        glutSetWindow(main_window);

    glutPostRedisplay(); 
}

// Process all input from GLUI components
void Control(int control_id)
{
    Node * curr_node = scenegraph.getCurrentNode();
    int old_children_vec_size = curr_node->getChildCount();

    switch(control_id)
    {
        case CHILD_NODE_SELECT_B_ID:
        {
            // Select child node

            // Grab child given index
            Node * child_node = curr_node->getChild(child_node_index);

            // Set as new current node
            if (child_node != NULL)
                scenegraph.setCurrentNode(child_node);

            break;
        }
        case PARENT_NODE_SELECT_B_ID:
        {
            // Select parent node

            // Grab parent
            Node * parent_node = curr_node->getParent();

            // Set as new current node
            if (parent_node != NULL)
                scenegraph.setCurrentNode(parent_node);

            break;
        }
        case NODE_TYPE_LB_ID:
        {
            // The only thing we need is for the GUI to update
            break;
        }
        case TRANSFORM_TYPE_LB_ID:
        {
            // Select transform type

            string curr_transform_type = ((TransformNode *)curr_node)->getTransformType();
            string transform_type = transform_type_list[transform_type_index];
            glm::vec3 xyz;
            GLfloat theta = 0.0;

            // Make sure not to update parameters if we haven't changed types
            if (transform_type == curr_transform_type)
                break;

            // Set appropriate defaults for selected transform type
            if (transform_type == "Scale" || transform_type == "Rotate")
            {
                xyz.x = 1.0;
                xyz.y = 1.0;
                xyz.z = 1.0;

                ((TransformNode *)curr_node)->setParams(transform_type, xyz, theta);
            }
            else if (transform_type == "Translate")
            {
                xyz.x = 0.0;
                xyz.y = 0.0;
                xyz.z = 0.0;

                ((TransformNode *)curr_node)->setParams(transform_type, xyz, theta);
            }

            break;
        }
        case CHILD_NODE_ADD_B_ID:
        {
            // Add child node
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
                    // Make sure we can even add another light node
                    GLenum light_id;
                    if ((light_id = scenegraph.addLight()) != GL_INVALID_ENUM)
                        LightNode * light_node = new LightNode(light_id, curr_node);

                    break;
                }
            }
            break;
        }
        case PARENT_NODE_ADD_B_ID:
        {
            // Inject parent node
            switch(node_type_index)
            {
                case 0:
                {
                    ObjectNode * object_node = new ObjectNode();
                    curr_node->addParent(object_node);
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
            }
            break;
        }
        case CURR_NODE_UPDATE_B_ID:
        {
            // Update the current node

            string curr_node_type = curr_node->getNodeType();

            // Given current node type, update parameters
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
                glm::vec3 xyz = glm::vec3(x_param->get_float_val(), y_param->get_float_val(), z_param->get_float_val());
                GLfloat theta = theta_param->get_float_val();
                bool animation_flag = animation_param->get_int_val();

                ((TransformNode *)curr_node)->setParams(transform_type, xyz, theta, animation_flag);
            }
            else if (curr_node_type == "Camera")
            {
                float fov = fov_param->get_float_val();
                int vx = vx_param->get_int_val();
                int vy = vy_param->get_int_val();

                // Make sure we don't allow for wacky vx, vy, or fov values
                if ((vx >= 0 && vx <= g_width) && (vy >= 0 && vy <= g_height) && (fov >= 22.5 && fov <= 67.5))
                {
                    ((CameraNode *)curr_node)->setFOV(fov);
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
            // Delete current node

            // Grab parent
            Node * parent = curr_node->getParent();
            string curr_node_type  = curr_node->getNodeType();

            // Ensure deletion is even appropriate
            if (parent != NULL && curr_node != scenegraph.getRootNode() && curr_node_type != "Camera")
            {
                // We need to ensure at least one light is in the scene
                if (curr_node_type == "Light" && !scenegraph.removeLight(((LightNode *)curr_node)->getLightId()))
                    break;

                // Remove node and then set the current node to its parent node
                curr_node->removeNode();
                scenegraph.setCurrentNode(parent);
            }
            break;
        }
    }

    // Update GUI given current node
    UpdateGUI(old_children_vec_size);
    
    glutPostRedisplay();
}