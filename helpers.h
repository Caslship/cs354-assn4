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
    glutInitWindowSize(g_width, g_height);
    main_window = glutCreateWindow("Jason Palacios - Scene Graph Viewer");
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
    glutReshapeFunc(Reshape);  
    glutKeyboardFunc(Keyboard);
    glutMouseFunc(MouseButton);
    glutMotionFunc(MouseMotion);
}

void InitializeGUI(void)
{
    glui = GLUI_Master.create_glui("GLUI");

    // Scene graph panel
    scene_graph_panel = glui->add_panel("Edit Scene Graph");

        // Node selection panel
        select_node_panel = glui->add_rollout_to_panel(scene_graph_panel, "Node Selection", true);
            
            // Child selection listbox
            child_node_select = glui->add_listbox_to_panel(select_node_panel, "Child ", &child_node_index);

            glui->add_separator_to_panel(select_node_panel);

            // Node selection buttons
            select_child_node = glui->add_button_to_panel(select_node_panel, "Select Child", CHILD_NODE_SELECT_B_ID, Control);
            select_parent_node = glui->add_button_to_panel(select_node_panel, "Select Parent", PARENT_NODE_SELECT_B_ID, Control);

        // Node addition panel
        add_node_panel = glui->add_rollout_to_panel(scene_graph_panel, "Node Addition", true);
            
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
        attr_node_panel = glui->add_rollout_to_panel(curr_node_panel, "Attribute", false);
            
            // Render mode selection listbox
            render_type_select = glui->add_listbox_to_panel(attr_node_panel, "Render ", &render_type_index);
            for(int i = 0; i < 6; i++ )
                render_type_select->add_item(i, render_type_list[i].c_str());

        // Geometry panel
        geom_node_panel = glui->add_rollout_to_panel(curr_node_panel, "Geometry", false);
            
            // Obj filepath textbox
            geom_path = glui->add_edittext_to_panel(geom_node_panel, "Obj", GLUI_EDITTEXT_TEXT);

        // Transform panel
        transform_node_panel = glui->add_rollout_to_panel(curr_node_panel, "Transformation", false);

            // Transform type selection listbox
            transform_type_select = glui->add_listbox_to_panel(transform_node_panel, "Type ", &transform_type_index, TRANSFORM_TYPE_LB_ID, Control);
            for(int i = 0; i < 3; i++)
                transform_type_select->add_item(i, transform_type_list[i].c_str());
            transform_type_select->set_alignment(GLUI_ALIGN_RIGHT);

            // Coordinate type selection listbox
            // transform_coord_type_select = glui->add_listbox_to_panel(transform_node_panel, "Coords ", &transform_coord_type_index);
            // for (int i = 0; i < 2; i++)
            //     transform_coord_type_select->add_item(i, transform_coord_type_list[i].c_str());
            // transform_coord_type_select->set_alignment(GLUI_ALIGN_RIGHT);

            glui->add_separator_to_panel(transform_node_panel);

            // Transformation parameter textboxes
            x_param = glui->add_edittext_to_panel(transform_node_panel, "X", GLUI_EDITTEXT_FLOAT);
            y_param = glui->add_edittext_to_panel(transform_node_panel, "Y", GLUI_EDITTEXT_FLOAT);
            z_param = glui->add_edittext_to_panel(transform_node_panel, "Z", GLUI_EDITTEXT_FLOAT);
            theta_param = glui->add_edittext_to_panel(transform_node_panel, "Theta", GLUI_EDITTEXT_FLOAT);

        // Camera panel
        camera_node_panel = glui->add_rollout_to_panel(curr_node_panel, "Camera", false);

            // Viewport parameter textboxes
            vx_param = glui->add_edittext_to_panel(camera_node_panel, "Viewport X", GLUI_EDITTEXT_INT);
            vy_param = glui->add_edittext_to_panel(camera_node_panel, "Viewport Y", GLUI_EDITTEXT_INT);

        // Light panel
        light_node_panel = glui->add_rollout_to_panel(curr_node_panel, "Light", false);

            // Light type selection listbox
            light_type_select = glui->add_listbox_to_panel(light_node_panel, "Type ", &light_type_index);
            for (int i = 0; i < 2; i++)
                light_type_select->add_item(i, light_type_list[i].c_str());

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
    // Start certain components off disabled and then enable ones that are appropriate given program state
    child_node_select->disable();
    select_child_node->disable();
    select_parent_node->disable();

    add_child_node->disable();
    add_parent_node->disable();

    attr_node_panel->disable();
    attr_node_panel->close();

    geom_node_panel->disable();
    geom_node_panel->close();

    transform_node_panel->disable();
    transform_node_panel->close();

    camera_node_panel->disable();
    camera_node_panel->close();

    light_node_panel->disable();
    light_node_panel->close();

    update_node->disable();
    delete_node->disable();

    Node * root_node = scenegraph.getRootNode();
    Node * curr_node = scenegraph.getCurrentNode();
    string curr_node_type = curr_node->getNodeType();
    vector<string> children_node_type_vec = curr_node->getChildNodeTypes();
    int children_vec_size = curr_node->getChildCount();
    int light_count = scenegraph.getLightCount();

    bool has_children = (children_vec_size > 0);
    bool is_root = (curr_node == root_node);
    bool is_geom_type = (curr_node_type == "Geometry");
    bool is_transform_type = (curr_node_type == "Transform");
    bool is_attr_type = (curr_node_type == "Attribute");
    bool is_camera_type = (curr_node_type == "Camera");
    bool is_light_type = (curr_node_type == "Light");
    bool camera_node_selected = (node_type_list[node_type_index] == "Camera");
    bool geom_node_selected = (node_type_list[node_type_index] == "Geometry");
    bool light_node_selected = (node_type_list[node_type_index] == "Light");
    bool rotation_node_selected  = (transform_type_list[transform_type_index] == "Rotate");

    for (int i = 0; i < old_children_vec_size; i++)
        child_node_select->delete_item(i);

    if (has_children)
    {
        for (int i = 0; i < children_vec_size; i++)
            child_node_select->add_item(i, children_node_type_vec[i].c_str());

        child_node_select->enable();
        select_child_node->enable();

        child_node_select->do_selection(0);
    }

    if (!is_root)
        select_parent_node->enable();

    if (!is_root && !camera_node_selected && !light_node_selected && !geom_node_selected)
        add_parent_node->enable();

    if (!is_camera_type && !is_geom_type && !is_light_type && !(light_node_selected && light_count == 8))
        add_child_node->enable();

    if (is_attr_type)
    {
        attr_node_panel->enable();
        attr_node_panel->open();

        render_type_select->enable();
    }
    else if (is_geom_type)
    {
        geom_node_panel->enable();
        geom_node_panel->open();

        geom_path->enable();

        geom_path->set_text(((GeometryNode *)curr_node)->getFilePath().c_str());
    }
    else if (is_transform_type)
    {
        transform_node_panel->enable();
        transform_node_panel->open();

        transform_type_select->enable();
        x_param->enable();
        y_param->enable();
        z_param->enable();
        theta_param->enable();

        if (!rotation_node_selected)
            theta_param->disable();

        x_param->set_float_val(((TransformNode *)curr_node)->getX());
        y_param->set_float_val(((TransformNode *)curr_node)->getY());
        z_param->set_float_val(((TransformNode *)curr_node)->getZ());
        theta_param->set_float_val(((TransformNode *)curr_node)->getTheta());
    }
    else if (is_camera_type)
    {
        camera_node_panel->enable();
        camera_node_panel->open();

        vx_param->enable();
        vy_param->enable();
    }
    else if (is_light_type)
    {
        light_node_panel->enable();
        light_node_panel->open();

        light_type_select->enable();
    }

    if (!is_camera_type && !is_root && !(is_light_type && light_count == 1))
        delete_node->enable();

    if (!is_root)
        update_node->enable();
}

#endif
