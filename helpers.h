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
            child_node_select = glui->add_listbox_to_panel(select_node_panel, "Child ", &child_node_index);

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
            render_type_select = glui->add_listbox_to_panel(attr_node_panel, "Render ", &render_type_index);
            for(int i = 0; i < 6; i++ )
                render_type_select->add_item(i, render_type_list[i].c_str());

        // Geometry panel
        geom_node_panel = glui->add_panel_to_panel(curr_node_panel, "Geometry");
            
            // Obj filepath textbox
            geom_path = glui->add_edittext_to_panel(geom_node_panel, "Obj", GLUI_EDITTEXT_TEXT);

        // Transform panel
        transform_node_panel = glui->add_panel_to_panel(curr_node_panel, "Transformation");

            // Transform type selection listbox
            transform_type_select = glui->add_listbox_to_panel(transform_node_panel, "Type ", &transform_type_index, TRANSFORM_TYPE_LB_ID, Control);
            for(int i = 0; i < 3; i++)
                transform_type_select->add_item(i, transform_type_list[i].c_str());

            // Coordinate type selection listbox
            transform_coord_type_select = glui->add_listbox_to_panel(transform_node_panel, "Coords ", &transform_coord_type_index);
            for (int i = 0; i < 2; i++)
                transform_coord_type_select->add_item(i, transform_coord_type_list[i].c_str());

            glui->add_separator_to_panel(transform_node_panel);

            // Transformation parameter textboxes
            x_param = glui->add_edittext_to_panel(transform_node_panel, "X", GLUI_EDITTEXT_FLOAT);
            y_param = glui->add_edittext_to_panel(transform_node_panel, "Y", GLUI_EDITTEXT_FLOAT);
            z_param = glui->add_edittext_to_panel(transform_node_panel, "Z", GLUI_EDITTEXT_FLOAT);
            theta_param = glui->add_edittext_to_panel(transform_node_panel, "Theta", GLUI_EDITTEXT_FLOAT);

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
    render_type_select->enable();
    geom_node_panel->enable();
    geom_path->enable();
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

    if (curr_node == root_node || node_type_list[node_type_index] == "Light" || node_type_list[node_type_index] == "Geometry")
        add_parent_node->disable();

    if (curr_node_type == "Camera" || curr_node_type == "Geometry" || curr_node_type == "Light")
        add_child_node->disable();

    if (curr_node_type != "Attribute")
        attr_node_panel->disable();

    if (curr_node_type != "Geometry")
        geom_node_panel->disable();
    else
    {
        geom_path->set_text(((GeometryNode *)curr_node)->getFilePath().c_str());
    }

    if (curr_node_type != "Transform")
        transform_node_panel->disable();
    else
    {
        x_param->set_float_val(((TransformNode *)curr_node)->getX());
        y_param->set_float_val(((TransformNode *)curr_node)->getY());
        z_param->set_float_val(((TransformNode *)curr_node)->getZ());
        theta_param->set_float_val(((TransformNode *)curr_node)->getTheta());

        if (transform_type_list[transform_type_index] != "Rotate")
            theta_param->disable();
    }

    if (curr_node_type == "Camera" || curr_node == root_node)
        delete_node->disable();

    if (curr_node_type == "Light" || curr_node == root_node)
        update_node->disable();
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
