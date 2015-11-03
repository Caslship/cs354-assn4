/*
* Assignment #4
* Name: Jason Palacios
* UT EID: jap4839
* UTCS: jason777
*/

#ifndef __HELPERS_H__
#define __HELPERS_H__

#include <GL/glut.h>
#include <GL/glui.h>
#include <fstream>
#include <iostream>
#include <string>
#include "enums.h"
#include "callbacks.h"
#include "scenegraph.h"
#include "geom.h"
#include "loader.h"

using namespace std;

// For some reason, having these as local objects cause segmentation faults
static TrimeshLoader model_loader;
static ifstream ifs;
static string prog_switch = "";
static string file_name = "";
static string x = "";
static string y = "";
static string z = "";
static string angle = "";

// Initalize GLUT and window properties
void InitializeWindow(int& argc, char ** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(win_width, win_height);
    main_window = glutCreateWindow("Jason Palacios - Scene Graph Manager");
}

// Initial settings for OpenGL
void InitializeGraphics(void)
{
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glShadeModel(GL_SMOOTH);
}

// Register all callbacks to be used with GLUT
void RegisterCallbacks(void)
{
    glutDisplayFunc(Display);
    GLUI_Master.set_glutReshapeFunc(Reshape);  
    GLUI_Master.set_glutKeyboardFunc(Keyboard);
    GLUI_Master.set_glutMouseFunc(MouseButton);
    glutMotionFunc(MouseMotion);
}

void InitializeGUI(void)
{
    glui = GLUI_Master.create_glui_subwindow(main_window, GLUI_SUBWINDOW_LEFT);

    // Scene graph panel
    scene_graph_panel = glui->add_panel("Edit Scene Graph");

        // Node selection panel
        select_node_panel = glui->add_panel_to_panel(scene_graph_panel, "Node Selection");
            
            // Child selection listbox
            child_node_select = glui->add_listbox_to_panel(select_node_panel, "Child ", &child_node_index, CHILD_NODE_LB_ID, Control);

            glui->add_separator_to_panel(select_node_panel);

            // Node selection buttons
            select_child_node = glui->add_button_to_panel(select_node_panel, "Select Child", CHILD_NODE_SELECT_B_ID, Control);
            select_parent_node = glui->add_button_to_panel(select_node_panel, "Select Parent", PARENT_NODE_SELECT_B_ID, Control);

        // Node addition panel
        add_node_panel = glui->add_panel_to_panel(scene_graph_panel, "Node Addition");
            
            // Node type selection listbox
            node_type_select = glui->add_listbox_to_panel(add_node_panel, "Type ", &node_type_index, NODE_TYPE_LB_ID, Control);
            for (int i = 0; i < 5; i++)
                node_type_select->add_item(i, node_type_list[i].c_str());

            glui->add_separator_to_panel(add_node_panel);

            // Node addition buttons
            add_child_node = glui->add_button_to_panel(add_node_panel, "Add Child", CHILD_NODE_ADD_B_ID, Control);
            add_parent_node = glui->add_button_to_panel(add_node_panel, "Add Parent", PARENT_NODE_ADD_B_ID, Control);

    scene_graph_panel->set_alignment(GLUI_ALIGN_LEFT);

    // Current node panel
    curr_node_panel = glui->add_panel("Edit Current Node");

        // Attribute panel
        attr_node_panel = glui->add_panel_to_panel(curr_node_panel, "Attribute");
            
            // Render mode selection listbox
            render_mode_select = glui->add_listbox_to_panel(attr_node_panel, "Render ");
            for(int i = 0; i < 6; i++ )
                render_mode_select->add_item(i, render_mode_list[i].c_str());

        // Geometry panel
        geom_node_panel = glui->add_panel_to_panel(curr_node_panel, "Geometry");
            
            // Obj filepath textbox
            geometry_path = glui->add_edittext_to_panel(geom_node_panel, "Obj");

        // Transform panel
        transform_node_panel = glui->add_panel_to_panel(curr_node_panel, "Transformation");

            // Transform type selection listbox
            transform_type_select = glui->add_listbox_to_panel(transform_node_panel, "Type ");
            for(int i = 0; i < 3; i++)
                transform_type_select->add_item(i, transform_type_list[i].c_str());

            // Coordinate type selection listbox
            transform_coord_type_select = glui->add_listbox_to_panel(transform_node_panel, "Coords ");
            for (int i = 0; i < 2; i++)
                transform_coord_type_select->add_item(i, transform_coord_type_list[i].c_str());

            glui->add_separator_to_panel(transform_node_panel);

            // Transformation parameter textboxes
            x_param = glui->add_edittext_to_panel(transform_node_panel, "X");
            y_param = glui->add_edittext_to_panel(transform_node_panel, "Y");
            z_param = glui->add_edittext_to_panel(transform_node_panel, "Z");
            theta_param = glui->add_edittext_to_panel(transform_node_panel, "Theta");

        // Apply node modification buttons
        update_node = glui->add_button_to_panel(curr_node_panel, "Update", CURR_NODE_UPDATE_B_ID, Control);
        delete_node = glui->add_button_to_panel(curr_node_panel, "Delete", CURR_NODE_DELETE_B_ID, Control);

    curr_node_panel->set_alignment(GLUI_ALIGN_LEFT);

    // Update GUI according to current node
    UpdateGUI(0);


    // Link windows to GLUI, and register idle callback
    glui->set_main_gfx_window(main_window);
    GLUI_Master.set_glutIdleFunc(Idle);
}

void UpdateGUI(int old_children_vec_size)
{
    Node * root_node = scenegraph.getRootNode();
    Node * curr_node = scenegraph.getCurrentNode();

    // Start every component off in an enabled state
    scene_graph_panel->enable();
    select_node_panel->enable();
    child_node_select->enable();
    select_child_node->enable();
    select_parent_node->enable();
    add_node_panel->enable();
    node_type_select->enable();
    add_child_node->enable();
    add_parent_node->enable();
    curr_node_panel->enable();
    attr_node_panel->enable();
    render_mode_select->enable();
    geom_node_panel->enable();
    geometry_path->enable();
    transform_node_panel->enable();
    transform_type_select->enable();
    transform_coord_type_select->enable();
    x_param->enable();
    y_param->enable();
    z_param->enable();
    theta_param->enable();
    update_node->enable();
    delete_node->enable();

    string curr_node_type = curr_node->getNodeType();
    vector<string> children_node_type_vec = curr_node->getChildNodeTypes();

    for (int i = 0; i < old_children_vec_size; i++)
        child_node_select->delete_item(i);

    int children_vec_size = curr_node->getChildCount();
    if (children_vec_size > 0)
    {
        for (int i = 0; i < children_vec_size; i++)
            child_node_select->add_item(i, children_node_type_vec[i].c_str());
    }
    else
    {
        child_node_select->disable();
    }

    if (children_vec_size <= 0)
        select_child_node->disable();

    if (curr_node == root_node)
        select_parent_node->disable();

    if (curr_node == root_node)
        add_parent_node->disable();

    if (curr_node_type == "Camera" || curr_node_type == "Geometry" || curr_node_type == "Light")
        add_child_node->disable();

    if (curr_node_type != "Attribute")
        attr_node_panel->disable();

    if (curr_node_type != "Geometry")
        geom_node_panel->disable();

    if (curr_node_type != "Transform")
        transform_node_panel->disable();
    else
    {
        if (((TransformNode *)curr_node)->getTransformType() != ROTATE)
            theta_param->disable();
    }

    if (curr_node_type == "Camera")
        delete_node->disable();

    if (curr_node_type == "Light")
        update_node->disable();
}

// Executes a command and the return value indicates success or failure
bool ExecuteCommand(void)
{
    int pos = 0;
    int command_length = command.length();

    // Skip spaces but make sure not to hit the end of the string
    while (pos < command_length && command[pos] ==  ' ')
        pos++;

    if (pos == command_length)
        return false;

    // Read command type and update position
    char command_type = command[pos++];

    // Don't want anybody excluding spaces
    if (pos != command_length && command[pos] != ' ')
        return false;

    command_type = toupper(command_type);

    switch(command_type)
    {
        case CLI_LOAD:
        {
            // Skip spaces but make sure not to hit the end of the string 
            while (pos < command_length && command[pos] ==  ' ')
                pos++;

            if (pos == command_length)
                return false;

            // Read file name and update position
            file_name = "";
            while (pos < command_length && command[pos] != ' ')
            {
                file_name += command[pos++];
            }

            // Append .obj extension to the string if needed
            int ext_pos = file_name.find(".obj");
            if (ext_pos == string::npos || ext_pos != file_name.length() - 4)
                file_name += ".obj";

            // Load object file into Trimesh object
            Trimesh model;

            // Fail if the object failed to load
            if (!model_loader.loadOBJ(file_name.c_str(), &model))
                return false;

            // Adjust camera for new object
            look_at_pos = model.getCenter();
            zoom_level = model.getBoundingLength();
            zoom_level = (zoom_level < VIEWING_DISTANCE_MIN ? VIEWING_DISTANCE_MIN : zoom_level);
            orbit_theta = orbit_phi = orbit_delta = pan_x = pan_y = 0.0;
            g_width = win_width;
            g_height = win_height;

            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            gluPerspective(45.0, (float)g_width / (float)g_height, g_near_plane, g_far_plane);

            glMatrixMode(GL_MODELVIEW);

            break;
        }
        case CLI_DELETE:
        {
            if (false)
            {
                // Reset camera for old object
                if (false)
                {
                    Trimesh model;

                    look_at_pos = model.getCenter();
                    zoom_level = model.getBoundingLength();
                    zoom_level = (zoom_level < VIEWING_DISTANCE_MIN ? VIEWING_DISTANCE_MIN : zoom_level);
                    orbit_theta = orbit_phi = orbit_delta = pan_x = pan_y = 0.0;
                    g_width = win_width;
                    g_height = win_height;

                    glMatrixMode(GL_PROJECTION);
                    glLoadIdentity();
                    gluPerspective(45.0, (float)g_width / (float)g_height, g_near_plane, g_far_plane);

                    glMatrixMode(GL_MODELVIEW);
                }
            }

            break;
        }
        case CLI_IDENTITY:
        {
            if (false)
            {
                if (trans_flag == WORLD_COORDS)
                {
                    // Clear transformations for object
                }
                else if (trans_flag == VIEW_COORDS)
                {
                    Trimesh model;

                    // Clear transformations for view
                    look_at_pos = model.getCenter();
                    zoom_level = model.getBoundingLength();
                    zoom_level = (zoom_level < VIEWING_DISTANCE_MIN ? VIEWING_DISTANCE_MIN : zoom_level);
                    orbit_theta = orbit_phi = orbit_delta = pan_x = pan_y = 0.0;
                    g_width = win_width;
                    g_height = win_height;

                    glMatrixMode(GL_PROJECTION);
                    glLoadIdentity();
                    gluPerspective(45.0, (float)g_width / (float)g_height, g_near_plane, g_far_plane);

                    glMatrixMode(GL_MODELVIEW);
                }
            }

            break;
        }
        case CLI_TRANSLATE:
        case CLI_SCALE:
        case CLI_ROTATE:
        {
            float ang = 0.0;
            float xyz[3] = {};

            // Read angle first if we have a rotate command
            if (command_type == CLI_ROTATE)
            {
                // Skip spaces but make sure not to hit the end of the string 
                while (pos < command_length && command[pos] ==  ' ')
                    pos++;

                if (pos == command_length)
                    return false;

                // Read ANGLE and update position
                angle = "";
                while (pos < command_length && command[pos] != ' ')
                {
                    angle += command[pos++];
                }             

                // Parse ANG from string
                ang = atof(angle.c_str());
            }

            // Skip spaces but make sure not to hit the end of the string 
            while (pos < command_length && command[pos] ==  ' ')
                pos++;

            if (pos == command_length)
                return false;

            // Read X and update position
            x = "";
            while (pos < command_length && command[pos] != ' ')
            {
                x += command[pos++];
            }

            // Skip spaces but make sure not to hit the end of the string 
            while (pos < command_length && command[pos] ==  ' ')
                pos++;

            if (pos == command_length)
                return false;

            // Read Y and update position
            y = "";
            while (pos < command_length && command[pos] != ' ')
            {
                y += command[pos++];
            }

            // Skip spaces but make sure not to hit the end of the string 
            while (pos < command_length && command[pos] ==  ' ')
                pos++;

            if (pos == command_length)
                return false;

            // Read Z and update position
            z = "";
            while (pos < command_length && command[pos] != ' ')
            {
                z += command[pos++];
            }

            // Parse XYZ from string
            xyz[0] = atof(x.c_str());
            xyz[1] = atof(y.c_str());
            xyz[2] = atof(z.c_str());

            if (trans_flag == WORLD_COORDS)
            {
            }
            else if (trans_flag == VIEW_COORDS)
            {
                // Add transformation to view
                if (command_type == CLI_TRANSLATE)
                {
                    // Pan
                    pan_x += xyz[0];
                    pan_y += xyz[1];
                    zoom_level += -xyz[2];
                }
                else if (command_type == CLI_SCALE)
                {
                    // Zoom
                    // Don't want a divide by zero or objects disappearing
                    if ((xyz[0] == 0.0) || (xyz[1] == 0.0) || (xyz[2] == 0.0))
                        return false;

                    zoom_level *= (xyz[2] * xyz[1] * xyz[0]);
                    g_width *= xyz[0];
                    g_height *= xyz[1];

                    glMatrixMode(GL_PROJECTION);
                    glLoadIdentity();
                    gluPerspective(45.0, (float)g_width / (float)g_height, g_near_plane, g_far_plane);

                    glMatrixMode(GL_MODELVIEW);
                }
                else if (command_type == CLI_ROTATE)
                {
                    // Orbit
                    orbit_phi += xyz[0] * ang;
                    orbit_theta -= xyz[1] * ang;
                    orbit_delta += xyz[2] * ang;
                }
            }

            break;
        }
        case CLI_VIEW:
        case CLI_WORLD:
        {
            // Set coordinate system to use
            trans_flag = (command_type == CLI_VIEW ? VIEW_COORDS : WORLD_COORDS);
            
            break;
        }
        case CLI_MODE:
        {
            // Skip spaces but make sure not to hit the end of the string
            while (pos < command_length && command[pos] ==  ' ')
                pos++;

            if (pos == command_length)
                return false;

            // Read mode type
            char mode_type = command[pos++];

            // Don't want anybody trying to trick up the CLI
            if (pos < command_length && command[pos] != ' ')
            {
                return false;
            }

            mode_type = toupper(mode_type);

            bool found_mode = false;
            for (int i = 0; i < 6; i++)
            {
                if ((found_mode = (mode_type == render_map[i])))
                {
                    //curr_render = i;
                    break;
                }
            }

            if (found_mode)
                break;
            else
                return false;
        }
        case CLI_EXIT:
        {
            // Quit program
            exit(EXIT_SUCCESS);
            break;
        }
        default:
            return false;
    }

    return true;
}

// Set camera view depending current zoom, pan, and orbit settings
void SetCameraView(void)
{
    if (false)
    {
        vertex_t model_center;
        float pos[0] = {};
        model_center.setPos(pos);

        // Zoom
        gluLookAt(look_at_pos.pos[0], look_at_pos.pos[1], zoom_level, look_at_pos.pos[0], look_at_pos.pos[1], look_at_pos.pos[2], 0.0, 1.0, 0.0);

        // Pan
        glTranslatef(-pan_x, -pan_y, 0.0);
        glTranslatef(model_center.pos[0], model_center.pos[1], model_center.pos[2] - zoom_level);

        // Orbit
        glRotatef(orbit_phi, 1.0, 0.0, 0.0);
        glRotatef(orbit_theta, 0.0, 1.0, 0.0);
        glRotatef(orbit_delta, 0.0, 0.0, 1.0);
        glTranslatef(-model_center.pos[0], -model_center.pos[1], -model_center.pos[2]);
    }
}

// Setup light position and direction
void SetLighting(void)
{
    // Set light to always be positioned behind the camera
    // g_light_pos[0] = camera_x;
    // g_light_pos[1] = camera_y;
    // g_light_pos[2] = zoom_level;
    // g_light_pos[3] = 1.0;
    // glLightfv(GL_LIGHT0, GL_POSITION, g_light_pos);

    // Set light to always point at whatever the camera is looking at
    g_light_direction[0] = 0.0;
    g_light_direction[1] = 0.0;
    g_light_direction[2] = look_at_pos.pos[2] - zoom_level;
    g_light_direction[3] = 1.0;
    glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, g_light_direction); 
}

#endif
